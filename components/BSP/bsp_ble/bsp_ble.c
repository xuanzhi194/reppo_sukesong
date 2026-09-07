#include "bsp_ble.h"
#include "reppo_demo_mode.h"
#include "todo_storage.h"
#include "badge_storage.h"
#include "rx8130.h"
#include "driver/i2c.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdbool.h>
#include <string.h>

extern volatile bool todo_ble_ui_refresh_label;
extern bool ble_success;
extern bool ble_enabled;
extern volatile bool ble_ui_refresh_req;
extern todo_list_t ui_todo_list;

/* NimBLE */
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "host/ble_hs_mbuf.h"
#include "host/ble_gatt.h"
#include "host/ble_att.h"
#include "host/ble_uuid.h"
#include "services/gap/ble_svc_gap.h"

void ble_store_config_init(void);

static const char *TAG = "REPPO_BLE_Device";

#define GATTS_SERVICE_UUID 0x00FF
#define GATTS_CHAR_UUID    0xFF01
/* 每条待办新增 plannedDate 字段后单条记录变长，缓冲区同步放大（位于 PSRAM） */
#define JSON_BUFFER_SIZE   16384
#define BLE_PREFERRED_MTU  512
#define BLE_SYNC_NOTIFY_DELAY_MS 150
#define BLE_TIME_STATUS_POLL_MS 50

#ifndef BLE_HS_CONN_HANDLE_NONE
#define BLE_HS_CONN_HANDLE_NONE 0xffff
#endif
#ifndef BLE_ATT_ERR_INSUFFICIENT_RES
#define BLE_ATT_ERR_INSUFFICIENT_RES 0x11
#endif

static uint8_t ble_addr_type;
static bool s_ble_active;
static uint16_t gatt_chr_val_handle;
static uint16_t active_conn_handle = BLE_HS_CONN_HANDLE_NONE;
static bool notify_enabled;
static bsp_ble_book_rx_t s_book_rx;

#if REPPO_DEMO_BLE_BOOT_GATE
static volatile bool s_demo_boot_gate_released;
#endif

static char json_buffer[JSON_BUFFER_SIZE] EXT_RAM_BSS_ATTR;
static uint16_t json_len;
static bool json_assembly_error;
static char stored_read_json[JSON_BUFFER_SIZE] EXT_RAM_BSS_ATTR = "{\"todo\":[],\"taskDone\":[]}";
static uint16_t stored_read_len;
static TaskHandle_t sync_notify_task_handle;

static void ble_app_advertise(void);
static int send_todo_json_via_notify(void);
static bool ble_refresh_stored_read_bundle(void);
static void schedule_sync_notify_delayed(void);
static void badge_notify_status(uint8_t status, uint8_t progress);
static void book_notify_status(uint8_t status, uint8_t progress);
static uint32_t ble_read_u32_le(const uint8_t *p);
static uint16_t ble_read_u16_le(const uint8_t *p);
static int notify_todo_chunk(uint8_t cmd, const uint8_t *payload, uint16_t payload_len);
static int time_notify_status(uint8_t status);
/* Only copy mailbox state under this lock; never do I2C or GATT calls in it. */
static portMUX_TYPE s_phone_time_mux = portMUX_INITIALIZER_UNLOCKED;
static rx8130_time_t s_pending_phone_time;
static bool s_phone_time_pending;
static bool s_phone_time_connected;
static bool s_phone_time_in_flight;
static bool s_phone_time_status_pending;
static uint8_t s_phone_time_status;
static uint32_t s_phone_time_session;
static struct ble_npl_callout s_phone_time_status_callout;

static bool ble_refresh_stored_read_bundle(void)
{
    /* Flush in-page complete/recover done flags before READ reloads from flash. */
    (void)todo_storage_apply_ui_done_flags(&ui_todo_list);

    esp_err_t err = todo_storage_export_ble_read_bundle(stored_read_json,
                                                        sizeof(stored_read_json));
    if (err == ESP_ERR_INVALID_SIZE) {
        ESP_LOGE(TAG, "BLE read bundle too large (max %u)", (unsigned)sizeof(stored_read_json));
        return false;
    }
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "BLE read bundle export failed");
        return false;
    }
    stored_read_len = (uint16_t)strlen(stored_read_json);
    return true;
}

static uint32_t ble_read_u32_le(const uint8_t *p)
{
    return (uint32_t)p[0]
         | ((uint32_t)p[1] << 8)
         | ((uint32_t)p[2] << 16)
         | ((uint32_t)p[3] << 24);
}

static uint16_t ble_read_u16_le(const uint8_t *p)
{
    return (uint16_t)p[0] | ((uint16_t)p[1] << 8);
}

static int weekday_from_ymd(int y, int m, int d)
{
    static const int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
    if (m < 3) {
        y -= 1;
    }
    return (y + y / 4 - y / 100 + y / 400 + t[m - 1] + d) % 7;
}

static int days_in_month(int year, int month)
{
    static const int days[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 2) {
        bool leap = ((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0);
        return leap ? 29 : 28;
    }
    if (month < 1 || month > 12) {
        return 0;
    }
    return days[month];
}

static bool phone_time_from_payload(const uint8_t *payload, uint16_t payload_len,
                                   rx8130_time_t *out)
{
    if (payload == NULL || out == NULL ||
        (payload_len != 7 && payload_len != 8)) {
        return false;
    }

    uint16_t year = ble_read_u16_le(payload);
    uint8_t month = payload[2];
    uint8_t date = payload[3];
    uint8_t hour = payload[4];
    uint8_t minute = payload[5];
    uint8_t second = payload[6];
    uint8_t wday;

    if (year < 2000 || year > 2099 ||
        month < 1 || month > 12 ||
        date < 1 || date > (uint8_t)days_in_month((int)year, (int)month) ||
        hour > 23 || minute > 59 || second > 59) {
        return false;
    }

    if (payload_len == 8 && payload[7] > 6) {
        return false;
    }
    /* The calendar is authoritative, including when the App supplies wday. */
    wday = (uint8_t)weekday_from_ymd((int)year, (int)month, (int)date);

    out->year = year;
    out->month = month;
    out->date = date;
    out->hour = hour;
    out->minute = minute;
    out->second = second;
    out->day = wday;
    return true;
}

/* Called only by the NimBLE host, so a reconnect cannot steal an old ACK. */
static int time_notify_status(uint8_t status)
{
    uint8_t payload[1] = { status };
    int rc = notify_todo_chunk(BLE_CMD_TIME_STATUS, payload, sizeof(payload));
    if (rc != 0 && rc != BLE_HS_ENOTCONN) {
        ESP_LOGW(TAG, "TIME STATUS notify failed: rc=%d", rc);
    }
    return rc;
}

static void time_sync_reset(bool connected)
{
    portENTER_CRITICAL(&s_phone_time_mux);
    ++s_phone_time_session;
    s_phone_time_connected = connected;
    s_phone_time_pending = false;
    s_phone_time_in_flight = false;
    s_phone_time_status_pending = false;
    portEXIT_CRITICAL(&s_phone_time_mux);
}

/* Deliver the RTC result on the host task, after the main-loop I2C write.
 * Keep it until notify is subscribed / available, or this connection ends. */
static void time_status_poll_cb(struct ble_npl_event *event)
{
    (void)event;
    portENTER_CRITICAL(&s_phone_time_mux);
    bool connected = s_phone_time_connected;
    bool pending = s_phone_time_status_pending;
    uint8_t status = s_phone_time_status;
    uint32_t session = s_phone_time_session;
    portEXIT_CRITICAL(&s_phone_time_mux);

    if (!connected) {
        return;
    }
    if (pending && time_notify_status(status) == 0) {
        portENTER_CRITICAL(&s_phone_time_mux);
        if (session == s_phone_time_session) {
            s_phone_time_status_pending = false;
            s_phone_time_in_flight = false;
        }
        portEXIT_CRITICAL(&s_phone_time_mux);
    }

    portENTER_CRITICAL(&s_phone_time_mux);
    bool retry = s_phone_time_connected && s_phone_time_in_flight;
    portEXIT_CRITICAL(&s_phone_time_mux);
    if (retry) {
        ble_npl_callout_reset(&s_phone_time_status_callout,
                             ble_npl_time_ms_to_ticks32(BLE_TIME_STATUS_POLL_MS));
    }
}

static void badge_notify_status(uint8_t status, uint8_t progress)
{
    if (!notify_enabled || active_conn_handle == BLE_HS_CONN_HANDLE_NONE) {
        return;
    }
    uint8_t payload[2] = { status, progress };
    for (int attempt = 0; attempt < 5; attempt++) {
        int rc = notify_todo_chunk(BLE_CMD_IMG_STATUS, payload, sizeof(payload));
        if (rc == 0) {
            return;
        }
        ESP_LOGW(TAG, "badge status notify failed rc=%d attempt=%d", rc, attempt + 1);
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

static void book_notify_status(uint8_t status, uint8_t progress)
{
    if (!notify_enabled || active_conn_handle == BLE_HS_CONN_HANDLE_NONE) {
        return;
    }
    uint8_t payload[2] = { status, progress };
    for (int attempt = 0; attempt < 5; attempt++) {
        int rc = notify_todo_chunk(BLE_CMD_BOOK_STATUS, payload, sizeof(payload));
        if (rc == 0) {
            return;
        }
        ESP_LOGW(TAG, "book status notify failed rc=%d attempt=%d", rc, attempt + 1);
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

static int device_write_cb(uint16_t conn_handle,
                           uint16_t attr_handle,
                           struct ble_gatt_access_ctxt *ctxt,
                           void *arg)
{
    if (ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR) {
        if (!ble_refresh_stored_read_bundle()) {
            ESP_LOGE(TAG, "Sync bundle send via READ failed: prepare error");
            return BLE_ATT_ERR_UNLIKELY;
        }
        if (ctxt->offset > stored_read_len) {
            return BLE_ATT_ERR_INVALID_OFFSET;
        }
        {
            uint16_t remain = stored_read_len - ctxt->offset;
            int rc = os_mbuf_append(ctxt->om, stored_read_json + ctxt->offset, remain);
            if (rc == 0) {
                ESP_LOGI(TAG,
                         "Sync bundle sent via READ ok offset=%u chunk=%u total=%u",
                         (unsigned)ctxt->offset, (unsigned)remain,
                         (unsigned)stored_read_len);
                if (ctxt->offset == 0) {
                    ESP_LOGI(TAG, "READ JSON sent (%u bytes)", (unsigned)stored_read_len);
                }
                return 0;
            }
            ESP_LOGE(TAG, "Sync bundle send via READ failed: mbuf rc=%d", rc);
            return BLE_ATT_ERR_INSUFFICIENT_RES;
        }
    }

    if (ctxt->op != BLE_GATT_ACCESS_OP_WRITE_CHR) {
        return BLE_ATT_ERR_REQ_NOT_SUPPORTED;
    }

    uint16_t chunk_len = OS_MBUF_PKTLEN(ctxt->om);
    uint8_t data[256];

    if (chunk_len == 0 || chunk_len > sizeof(data)) {
        ESP_LOGW(TAG, "GATT write rejected: invalid chunk len=%u", (unsigned)chunk_len);
        return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
    }

    os_mbuf_copydata(ctxt->om, 0, chunk_len, data);

    uint8_t cmd = data[0];
    uint8_t *payload = &data[1];
    uint16_t payload_len = chunk_len - 1;

    ESP_LOGI(TAG, "GATT write handle=%u len=%u cmd=0x%02X", attr_handle, chunk_len, cmd);

    if (cmd == 0x01) {
        if (payload_len > 0) {
            ESP_LOGI(TAG, "Pomodoro command: %d minutes", payload[0]);
        }
    } else if (cmd == 0x02) {
        if (payload_len > 0 && payload_len < JSON_BUFFER_SIZE) {
            memcpy(json_buffer, payload, payload_len);
            json_buffer[payload_len] = '\0';
            ESP_LOGI(TAG, "JSON single (0x02) received (%u bytes)", (unsigned)payload_len);
            if (todo_storage_apply_ble_json(json_buffer) == ESP_OK) {
                todo_ble_ui_refresh_label = true;
                ble_refresh_stored_read_bundle();
            }
        } else {
            ESP_LOGW(TAG, "JSON single (0x02) rejected: len=%u", (unsigned)payload_len);
            return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
        }
    } else if (cmd == 0x06) {
        if (!ble_refresh_stored_read_bundle()) {
            ESP_LOGE(TAG, "Todo pull 0x06 failed: prepare error");
        } else if (notify_enabled) {
            schedule_sync_notify_delayed();
        }
        /* notify 未开时由小程序 read 触发发送，成功日志在 READ 回调里打 */
    } else if (cmd == 0x03) {
        json_len = 0;
        json_assembly_error = false;
        if (payload_len >= JSON_BUFFER_SIZE) {
            json_assembly_error = true;
            ESP_LOGW(TAG, "JSON START rejected: chunk too large");
            return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
        }
        memcpy(json_buffer, payload, payload_len);
        json_len = payload_len;
        ESP_LOGI(TAG, "JSON START (%u bytes)", (unsigned)payload_len);
    } else if (cmd == 0x04) {
        if (json_assembly_error || json_len + payload_len >= JSON_BUFFER_SIZE) {
            json_assembly_error = true;
            json_len = 0;
            ESP_LOGW(TAG, "JSON chunk rejected: overflow at offset %u", (unsigned)json_len);
            return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
        }
        memcpy(&json_buffer[json_len], payload, payload_len);
        json_len += payload_len;
    } else if (cmd == 0x05) {
        if (json_assembly_error || json_len + payload_len >= JSON_BUFFER_SIZE) {
            json_assembly_error = true;
            json_len = 0;
            ESP_LOGW(TAG, "JSON END rejected: overflow");
            return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
        }
        memcpy(&json_buffer[json_len], payload, payload_len);
        json_len += payload_len;
        json_buffer[json_len] = '\0';
        ESP_LOGI(TAG, "JSON END received (%u bytes total)", (unsigned)json_len);

        if (todo_storage_apply_ble_json(json_buffer) == ESP_OK) {
            todo_ble_ui_refresh_label = true;
            ble_refresh_stored_read_bundle();
        }
        json_len = 0;
        json_assembly_error = false;
    } else if (cmd == BLE_CMD_IMG_INFO) {
        if (payload_len < 9) {
            ESP_LOGW(TAG, "IMG INFO rejected: len=%u", (unsigned)payload_len);
            badge_notify_status(BADGE_STATUS_ERR, 0);
            return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
        }
        uint32_t total = ble_read_u32_le(payload);
        uint32_t crc32 = ble_read_u32_le(payload + 4);
        uint8_t format = payload[8];
        esp_err_t err = badge_storage_rx_begin(total, crc32, format);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "IMG INFO begin failed: %s", esp_err_to_name(err));
            badge_notify_status(BADGE_STATUS_ERR, 0);
            return BLE_ATT_ERR_UNLIKELY;
        }
        badge_notify_status(BADGE_STATUS_BUSY, 0);
    } else if (cmd == BLE_CMD_IMG_DATA) {
        if (payload_len < 5) {
            ESP_LOGW(TAG, "IMG DATA rejected: len=%u", (unsigned)payload_len);
            return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
        }
        uint32_t offset = ble_read_u32_le(payload);
        const uint8_t *chunk = payload + 4;
        uint16_t chunk_len = payload_len - 4;
        esp_err_t err = badge_storage_rx_write(offset, chunk, chunk_len);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "IMG DATA write failed off=%lu", (unsigned long)offset);
            badge_notify_status(BADGE_STATUS_ERR, 0);
            badge_storage_rx_abort();
            return BLE_ATT_ERR_UNLIKELY;
        }
    } else if (cmd == BLE_CMD_IMG_COMMIT) {
        esp_err_t err = badge_storage_rx_commit();
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "IMG COMMIT failed: %s", esp_err_to_name(err));
            badge_notify_status(BADGE_STATUS_ERR, 0);
            return BLE_ATT_ERR_UNLIKELY;
        }
        badge_notify_status(BADGE_STATUS_OK, 100);
        ESP_LOGI(TAG, "Badge image saved");
    } else if (cmd == BLE_CMD_BOOK_INFO) {
        if (s_book_rx.begin == NULL) {
            ESP_LOGW(TAG, "BOOK INFO: no rx handler");
            book_notify_status(BLE_BOOK_STATUS_ERR, 0);
            return BLE_ATT_ERR_UNLIKELY;
        }
        if (payload_len < 9) {
            ESP_LOGW(TAG, "BOOK INFO rejected: len=%u", (unsigned)payload_len);
            book_notify_status(BLE_BOOK_STATUS_ERR, 0);
            return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
        }
        uint32_t total = ble_read_u32_le(payload);
        uint32_t crc32 = ble_read_u32_le(payload + 4);
        uint8_t format = payload[8];
        esp_err_t err = s_book_rx.begin(total, crc32, format);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "BOOK INFO begin failed: %s", esp_err_to_name(err));
            book_notify_status(BLE_BOOK_STATUS_ERR, 0);
            return BLE_ATT_ERR_UNLIKELY;
        }
        book_notify_status(BLE_BOOK_STATUS_BUSY, 0);
    } else if (cmd == BLE_CMD_BOOK_DATA) {
        if (s_book_rx.write == NULL) {
            return BLE_ATT_ERR_UNLIKELY;
        }
        if (payload_len < 5) {
            ESP_LOGW(TAG, "BOOK DATA rejected: len=%u", (unsigned)payload_len);
            return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
        }
        uint32_t offset = ble_read_u32_le(payload);
        const uint8_t *chunk = payload + 4;
        uint16_t book_chunk_len = payload_len - 4;
        esp_err_t err = s_book_rx.write(offset, chunk, book_chunk_len);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "BOOK DATA write failed off=%lu", (unsigned long)offset);
            book_notify_status(BLE_BOOK_STATUS_ERR, 0);
            if (s_book_rx.abort != NULL) {
                s_book_rx.abort();
            }
            return BLE_ATT_ERR_UNLIKELY;
        }
    } else if (cmd == BLE_CMD_BOOK_COMMIT) {
        if (s_book_rx.commit == NULL) {
            book_notify_status(BLE_BOOK_STATUS_ERR, 0);
            return BLE_ATT_ERR_UNLIKELY;
        }
        esp_err_t err = s_book_rx.commit();
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "BOOK COMMIT failed: %s", esp_err_to_name(err));
            book_notify_status(BLE_BOOK_STATUS_ERR, 0);
            return BLE_ATT_ERR_UNLIKELY;
        }
        book_notify_status(BLE_BOOK_STATUS_OK, 100);
        ESP_LOGI(TAG, "Ebook saved");
    } else if (cmd == BLE_CMD_TIME_SET) {
        /* There is no request ID on the wire: finish one sync before the next. */
        portENTER_CRITICAL(&s_phone_time_mux);
        bool busy = s_phone_time_in_flight;
        bool connected = s_phone_time_connected;
        portEXIT_CRITICAL(&s_phone_time_mux);
        if (!connected || conn_handle != active_conn_handle) {
            return BLE_ATT_ERR_UNLIKELY;
        }
        if (busy) {
            return BLE_ATT_ERR_INSUFFICIENT_RES;
        }
        rx8130_time_t phone_time = {0};
        if (!phone_time_from_payload(payload, payload_len, &phone_time)) {
            ESP_LOGW(TAG, "TIME SET rejected: len=%u", (unsigned)payload_len);
            (void)time_notify_status(BLE_TIME_STATUS_ERR);
            return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
        }
        portENTER_CRITICAL(&s_phone_time_mux);
        s_pending_phone_time = phone_time;
        s_phone_time_pending = true;
        s_phone_time_in_flight = true;
        portEXIT_CRITICAL(&s_phone_time_mux);
        ble_npl_callout_reset(&s_phone_time_status_callout,
                             ble_npl_time_ms_to_ticks32(BLE_TIME_STATUS_POLL_MS));
        ESP_LOGI(TAG, "TIME SET queued %04u-%02u-%02u %02u:%02u:%02u wday=%u",
                 (unsigned)phone_time.year, (unsigned)phone_time.month,
                 (unsigned)phone_time.date, (unsigned)phone_time.hour,
                 (unsigned)phone_time.minute, (unsigned)phone_time.second,
                 (unsigned)phone_time.day);
    } else if (cmd == BLE_CMD_DEMO_BOOT_START) {
#if REPPO_DEMO_BLE_BOOT_GATE
        ESP_LOGI(TAG, "Demo boot START (0x30) — release normal boot");
        bsp_ble_demo_boot_gate_release();
#else
        ESP_LOGW(TAG, "Demo boot cmd ignored (REPPO_DEMO_BLE_BOOT_GATE=0)");
#endif
    }

    return 0;
}

static const struct ble_gatt_svc_def gatts_defs[] = {
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(GATTS_SERVICE_UUID),
        .characteristics = (struct ble_gatt_chr_def[]){
            {
                .uuid = BLE_UUID16_DECLARE(GATTS_CHAR_UUID),
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE |
                         BLE_GATT_CHR_F_WRITE_NO_RSP | BLE_GATT_CHR_F_NOTIFY,
                .val_handle = &gatt_chr_val_handle,
                .access_cb = device_write_cb,
            },
            { 0 },
        },
    },
    { 0 },
};

static int bsp_ble_gap_event(struct ble_gap_event *event, void *arg)
{
    switch (event->type) {
    case BLE_GAP_EVENT_CONNECT:
        ESP_LOGI(TAG, "BLE GAP EVENT CONNECT %s", event->connect.status == 0 ? "OK" : "Failed");
        if (event->connect.status == 0) {
            active_conn_handle = event->connect.conn_handle;
            notify_enabled = false;
            time_sync_reset(true);
            ble_success = true;
            ble_ui_refresh_req = true;
            ESP_LOGI(TAG, "Char 0x%04X value handle=%u (READ primary, NOTIFY optional)",
                     GATTS_CHAR_UUID, gatt_chr_val_handle);
            if (!ble_refresh_stored_read_bundle()) {
                ESP_LOGE(TAG, "Sync bundle prepare failed on connect");
            }
        } else {
            ble_app_advertise();
        }
        break;
    case BLE_GAP_EVENT_DISCONNECT:
        ESP_LOGI(TAG, "BLE GAP EVENT DISCONNECTED");
        badge_storage_rx_abort();
        if (s_book_rx.abort != NULL) {
            s_book_rx.abort();
        }
        time_sync_reset(false);
        ble_npl_callout_stop(&s_phone_time_status_callout);
        active_conn_handle = BLE_HS_CONN_HANDLE_NONE;
        notify_enabled = false;
        ble_success = false;
        ble_ui_refresh_req = true;
        ble_app_advertise();
        break;
    case BLE_GAP_EVENT_SUBSCRIBE:
        ESP_LOGI(TAG, "subscribe attr=%u notify=%d (chr=%u)",
                 event->subscribe.attr_handle, event->subscribe.cur_notify, gatt_chr_val_handle);
        if (event->subscribe.attr_handle == gatt_chr_val_handle ||
            gatt_chr_val_handle == 0) {
            notify_enabled = event->subscribe.cur_notify;
            ESP_LOGI(TAG, "Todo notify %s", notify_enabled ? "enabled" : "disabled");
            /* 不在 subscribe 回调里主动推送：等小程序完成监听/MTU 后再由 0x06 或 READ 拉取 */
        }
        break;
    case BLE_GAP_EVENT_MTU:
        ESP_LOGI(TAG, "MTU updated conn=%u mtu=%u",
                 event->mtu.conn_handle, event->mtu.value);
        break;
    case BLE_GAP_EVENT_ADV_COMPLETE:
        ESP_LOGI(TAG, "BLE GAP EVENT ADV COMPLETE");
        if (s_ble_active) {
            ble_app_advertise();
        }
        break;
    default:
        break;
    }
    return 0;
}

static int notify_todo_chunk(uint8_t cmd, const uint8_t *payload, uint16_t payload_len)
{
    uint8_t packet[20];

    if (!notify_enabled || active_conn_handle == BLE_HS_CONN_HANDLE_NONE) {
        return BLE_HS_ENOTCONN;
    }

    packet[0] = cmd;
    if (payload_len > 0) {
        memcpy(&packet[1], payload, payload_len);
    }

    struct os_mbuf *om = ble_hs_mbuf_from_flat(packet, payload_len + 1);
    if (!om) {
        return BLE_HS_ENOMEM;
    }

    return ble_gatts_notify_custom(active_conn_handle, gatt_chr_val_handle, om);
}

static int send_todo_json_via_notify(void)
{
    const uint8_t *bytes;
    uint16_t offset;
    const uint16_t max_payload = 19;

    if (!ble_refresh_stored_read_bundle()) {
        ESP_LOGE(TAG, "Sync bundle send via NOTIFY failed: prepare error");
        return -1;
    }

    if (!notify_enabled || active_conn_handle == BLE_HS_CONN_HANDLE_NONE) {
        return BLE_HS_ENOTCONN;
    }

    bytes = (const uint8_t *)stored_read_json;
    offset = 0;

    while (offset < stored_read_len) {
        uint16_t remain = stored_read_len - offset;
        uint16_t chunk_len = remain > max_payload ? max_payload : remain;
        uint8_t cmd;
        int rc;

        if (offset == 0 && chunk_len == stored_read_len) {
            cmd = 0x02;
        } else if (offset == 0) {
            cmd = 0x03;
        } else if (offset + chunk_len >= stored_read_len) {
            cmd = 0x05;
        } else {
            cmd = 0x04;
        }

        rc = notify_todo_chunk(cmd, &bytes[offset], chunk_len);
        if (rc != 0) {
            ESP_LOGE(TAG, "Sync bundle send via NOTIFY failed: cmd=0x%02X rc=%d", cmd, rc);
            return rc;
        }
        offset += chunk_len;
    }

    ESP_LOGI(TAG, "Sync bundle (todo+taskDone) sent to WeChat via NOTIFY ok (%u bytes)",
             (unsigned)stored_read_len);
    return 0;
}

static void sync_notify_delayed_task(void *arg)
{
    /*
     * A notification wakes this task early when BLE is being shut down.
     * This prevents delayed GATT calls from racing nimble_port_deinit().
     */
    bool cancelled =
        ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(BLE_SYNC_NOTIFY_DELAY_MS)) > 0;
    sync_notify_task_handle = NULL;

    if (cancelled || !s_ble_active || !notify_enabled ||
        active_conn_handle == BLE_HS_CONN_HANDLE_NONE) {
        ESP_LOGW(TAG, "Todo pull 0x06: delayed send skipped (conn/notify not ready)");
        vTaskDelete(NULL);
        return;
    }

    ESP_LOGI(TAG, "Todo pull 0x06: delayed %ums, sending notify",
             (unsigned)BLE_SYNC_NOTIFY_DELAY_MS);
    send_todo_json_via_notify();
    vTaskDelete(NULL);
}

static void schedule_sync_notify_delayed(void)
{
    if (!s_ble_active) {
        return;
    }
    if (sync_notify_task_handle != NULL) {
        ESP_LOGW(TAG, "Todo pull 0x06: sync notify already scheduled");
        return;
    }

    BaseType_t rc = xTaskCreate(sync_notify_delayed_task, "ble_sync_notify", 3072,
                                NULL, 5, &sync_notify_task_handle);
    if (rc != pdPASS) {
        sync_notify_task_handle = NULL;
        ESP_LOGE(TAG, "Todo pull 0x06: failed to schedule delayed notify");
    }
}

static void ble_app_advertise(void)
{
    struct ble_gap_adv_params adv_params;
    struct ble_hs_adv_fields fields;
    int rc;

    if (!s_ble_active) {
        return;
    }

    /*
     * Primary advertisement payload (25 / 31 bytes):
     *   Flags                  3 B  [02 01 06]
     *   Complete UUID16 list   4 B  [03 03 FF 00]   ← WeChat scans by service UUID
     *   Complete Local Name   18 B  [11 09 "REPPO_BLE_Device"]
     *
     * Full name is in the primary packet so Android/iOS both match it correctly.
     * No scan response needed — saves stack resources and avoids active-scan dependency.
     */
    static const ble_uuid16_t adv_uuids16[] = { BLE_UUID16_INIT(GATTS_SERVICE_UUID) };

    memset(&fields, 0, sizeof(fields));
    fields.flags               = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;
    fields.uuids16             = adv_uuids16;
    fields.num_uuids16         = 1;
    fields.uuids16_is_complete = 1;
    fields.name                = (uint8_t *)"REPPO_BLE_Device";
    fields.name_len            = 16;
    fields.name_is_complete    = 1;

    rc = ble_gap_adv_set_fields(&fields);
    if (rc != 0) {
        ESP_LOGE(TAG, "Advertising set fields failed: %d", rc);
        return;
    }

    memset(&adv_params, 0, sizeof(adv_params));
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;

    rc = ble_gap_adv_start(ble_addr_type, NULL, BLE_HS_FOREVER, &adv_params, bsp_ble_gap_event, NULL);
    if (rc != 0) {
        ESP_LOGE(TAG, "Advertising start failed: %d", rc);
    } else {
        ESP_LOGI(TAG, "BLE ready — scan \"REPPO_BLE_Device\" (svc UUID 0x%04X)", GATTS_SERVICE_UUID);
    }
}

static void ble_app_on_sync(void)
{
    ESP_LOGI(TAG, "BLE host synced");
    ble_hs_id_infer_auto(0, &ble_addr_type);
    ble_app_advertise();
}

static void ble_app_on_reset(int reason)
{
    ESP_LOGI(TAG, "Resetting state; reason=%d", reason);
    time_sync_reset(false);
    ble_npl_callout_stop(&s_phone_time_status_callout);
    active_conn_handle = BLE_HS_CONN_HANDLE_NONE;
    notify_enabled = false;
    ble_success = false;
    ble_ui_refresh_req = true;
}

static void ble_host_task(void *param)
{
    ESP_LOGI(TAG, "BLE Host Task Started");
    nimble_port_run();
    nimble_port_freertos_deinit();
}

bool bsp_ble_is_active(void)
{
    return s_ble_active;
}

bool bsp_ble_is_connected(void)
{
    return active_conn_handle != BLE_HS_CONN_HANDLE_NONE;
}

void bsp_ble_disconnect(void)
{
    if (active_conn_handle == BLE_HS_CONN_HANDLE_NONE) {
        return;
    }
    int rc = ble_gap_terminate(active_conn_handle, BLE_ERR_REM_USER_CONN_TERM);
    if (rc != 0) {
        ESP_LOGW(TAG, "ble_gap_terminate failed: %d", rc);
    }
}

void bsp_ble_register_book_rx(const bsp_ble_book_rx_t *rx)
{
    if (rx == NULL) {
        memset(&s_book_rx, 0, sizeof(s_book_rx));
        return;
    }
    s_book_rx = *rx;
}

void bsp_ble_init(void)
{
    int rc;

    if (s_ble_active) {
        ESP_LOGW(TAG, "BLE already active");
        return;
    }

    ESP_LOGI(TAG, "Initializing NimBLE...");

    rc = nimble_port_init();
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to initialize nimble port, rc=%d", rc);
        ble_enabled = false;
        ble_ui_refresh_req = true;
        return;
    }

    ble_hs_cfg.sync_cb = ble_app_on_sync;
    ble_hs_cfg.reset_cb = ble_app_on_reset;

    /* No ble_svc_gap_init — keep 0xFF01 at handle 3 for legacy WeChat writes */
    rc = ble_gatts_count_cfg(gatts_defs);
    if (rc != 0) {
        ESP_LOGE(TAG, "ble_gatts_count_cfg failed: %d", rc);
        nimble_port_deinit();
        ble_enabled = false;
        ble_ui_refresh_req = true;
        return;
    }

    rc = ble_gatts_add_svcs(gatts_defs);
    if (rc != 0) {
        ESP_LOGE(TAG, "ble_gatts_add_svcs failed: %d", rc);
        nimble_port_deinit();
        ble_enabled = false;
        ble_ui_refresh_req = true;
        return;
    }

    ble_svc_gap_device_name_set("REPPO_BLE_Device");
    ble_store_config_init();
    ble_att_set_preferred_mtu(BLE_PREFERRED_MTU);

    rc = ble_npl_callout_init(&s_phone_time_status_callout,
                              nimble_port_get_dflt_eventq(), time_status_poll_cb, NULL);
    if (rc != 0) {
        ESP_LOGE(TAG, "Time status callout init failed: %d", rc);
        nimble_port_deinit();
        ble_enabled = false;
        ble_ui_refresh_req = true;
        return;
    }
    time_sync_reset(false);

#if !REPPO_DEMO_BLE_BOOT_GATE
    ble_refresh_stored_read_bundle();
#endif
    s_ble_active = true;
    ble_enabled = true;
    ble_ui_refresh_req = true;

    nimble_port_freertos_init(ble_host_task);
}

void bsp_ble_deinit(void)
{
    if (!s_ble_active) {
        return;
    }

    s_ble_active = false;
    time_sync_reset(false);
    ble_npl_callout_stop(&s_phone_time_status_callout);

    if (sync_notify_task_handle != NULL) {
        xTaskNotifyGive(sync_notify_task_handle);
    }

    badge_storage_rx_abort();
    if (s_book_rx.abort != NULL) {
        s_book_rx.abort();
    }

    if (active_conn_handle != BLE_HS_CONN_HANDLE_NONE) {
        ble_gap_terminate(active_conn_handle, BLE_ERR_REM_USER_CONN_TERM);
        active_conn_handle = BLE_HS_CONN_HANDLE_NONE;
    }

    ble_gap_adv_stop();

    int rc = nimble_port_stop();
    if (rc == 0) {
        /* The host can no longer re-arm the callout; release it before NPL. */
        ble_npl_callout_stop(&s_phone_time_status_callout);
        ble_npl_callout_deinit(&s_phone_time_status_callout);
        nimble_port_deinit();
        ESP_LOGI(TAG, "NimBLE deinitialized");
    } else {
        ESP_LOGE(TAG, "NimBLE stop failed, rc=%d", rc);
    }

    notify_enabled = false;
    ble_success = false;
    ble_enabled = false;
    ble_ui_refresh_req = true;
}

bool bsp_ble_time_poll(void)
{
    portENTER_CRITICAL(&s_phone_time_mux);
    if (!s_phone_time_connected || !s_phone_time_pending) {
        portEXIT_CRITICAL(&s_phone_time_mux);
        return false;
    }

    rx8130_time_t phone_time = s_pending_phone_time;
    uint32_t session = s_phone_time_session;
    s_phone_time_pending = false;
    portEXIT_CRITICAL(&s_phone_time_mux);

    esp_err_t err = rx8130_set_time(I2C_NUM_0, &phone_time);
    portENTER_CRITICAL(&s_phone_time_mux);
    /* Ignore an in-progress old connection's result after disconnect/reset. */
    if (s_phone_time_connected && session == s_phone_time_session) {
        s_phone_time_status = err == ESP_OK ? BLE_TIME_STATUS_OK : BLE_TIME_STATUS_ERR;
        s_phone_time_status_pending = true;
    }
    portEXIT_CRITICAL(&s_phone_time_mux);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "TIME SET RTC write failed: %s", esp_err_to_name(err));
        return false;
    }

    ESP_LOGI(TAG, "RTC synced from phone %04u-%02u-%02u %02u:%02u:%02u",
             (unsigned)phone_time.year, (unsigned)phone_time.month,
             (unsigned)phone_time.date, (unsigned)phone_time.hour,
             (unsigned)phone_time.minute, (unsigned)phone_time.second);
    return true;
}

#if REPPO_DEMO_BLE_BOOT_GATE
void bsp_ble_demo_boot_gate_release(void)
{
    s_demo_boot_gate_released = true;
}

bool bsp_ble_demo_boot_gate_is_released(void)
{
    return s_demo_boot_gate_released;
}

void bsp_ble_demo_boot_gate_wait(void)
{
    ESP_LOGI(TAG, "Demo gate: waiting for BLE 0x30 or ENTER...");
    while (!s_demo_boot_gate_released) {
        vTaskDelay(pdMS_TO_TICKS(50));
    }
    ESP_LOGI(TAG, "Demo gate: released, continuing normal boot");
}
#endif
