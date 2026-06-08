#include "bsp_ble.h"
#include "todo_storage.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdbool.h>
#include <string.h>

extern bool todo_ble_ui_refresh_label;
extern bool ble_success;
extern bool ble_enabled;
extern bool ble_ui_refresh_req;

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
#define JSON_BUFFER_SIZE   8192
#define BLE_PREFERRED_MTU  512
#define BLE_SYNC_NOTIFY_DELAY_MS 150

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

static char json_buffer[JSON_BUFFER_SIZE] EXT_RAM_BSS_ATTR;
static uint16_t json_len;
static char stored_read_json[JSON_BUFFER_SIZE] EXT_RAM_BSS_ATTR = "{\"todo\":[],\"taskDone\":[]}";
static uint16_t stored_read_len;
static TaskHandle_t sync_notify_task_handle;

static void ble_app_advertise(void);
static int send_todo_json_via_notify(void);
static bool ble_refresh_stored_read_bundle(void);
static void schedule_sync_notify_delayed(void);

static bool ble_refresh_stored_read_bundle(void)
{
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
                    ESP_LOGI(TAG, "READ JSON: %s", stored_read_json);
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
        return 0;
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
            ESP_LOGI(TAG, "==== JSON RECEIVED single (0x02) (%u bytes) ====", (unsigned)payload_len);
            ESP_LOGI(TAG, "%s", json_buffer);
            if (todo_storage_apply_ble_json(json_buffer) == ESP_OK) {
                todo_ble_ui_refresh_label = true;
                ble_refresh_stored_read_bundle();
            }
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
        if (payload_len < JSON_BUFFER_SIZE) {
            memcpy(json_buffer, payload, payload_len);
            json_len = payload_len;
        }
        ESP_LOGI(TAG, "JSON START");
    } else if (cmd == 0x04) {
        if (json_len + payload_len < JSON_BUFFER_SIZE) {
            memcpy(&json_buffer[json_len], payload, payload_len);
            json_len += payload_len;
        }
    } else if (cmd == 0x05) {
        if (json_len + payload_len < JSON_BUFFER_SIZE) {
            memcpy(&json_buffer[json_len], payload, payload_len);
            json_len += payload_len;
        }
        json_buffer[json_len] = '\0';
        ESP_LOGI(TAG, "==== JSON RECEIVED (%u bytes) ====", (unsigned)json_len);
        ESP_LOGI(TAG, "%s", json_buffer);

        if (todo_storage_apply_ble_json(json_buffer) == ESP_OK) {
            todo_ble_ui_refresh_label = true;
            ble_refresh_stored_read_bundle();
        }
        json_len = 0;
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
    vTaskDelay(pdMS_TO_TICKS(BLE_SYNC_NOTIFY_DELAY_MS));
    sync_notify_task_handle = NULL;

    if (!notify_enabled || active_conn_handle == BLE_HS_CONN_HANDLE_NONE) {
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
        return;
    }

    ble_hs_cfg.sync_cb = ble_app_on_sync;
    ble_hs_cfg.reset_cb = ble_app_on_reset;

    /* No ble_svc_gap_init — keep 0xFF01 at handle 3 for legacy WeChat writes */
    rc = ble_gatts_count_cfg(gatts_defs);
    if (rc != 0) {
        ESP_LOGE(TAG, "ble_gatts_count_cfg failed: %d", rc);
        nimble_port_deinit();
        return;
    }

    rc = ble_gatts_add_svcs(gatts_defs);
    if (rc != 0) {
        ESP_LOGE(TAG, "ble_gatts_add_svcs failed: %d", rc);
        nimble_port_deinit();
        return;
    }

    ble_svc_gap_device_name_set("REPPO_BLE_Device");
    ble_store_config_init();
    ble_att_set_preferred_mtu(BLE_PREFERRED_MTU);

    ble_refresh_stored_read_bundle();
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

    if (active_conn_handle != BLE_HS_CONN_HANDLE_NONE) {
        ble_gap_terminate(active_conn_handle, BLE_ERR_REM_USER_CONN_TERM);
        active_conn_handle = BLE_HS_CONN_HANDLE_NONE;
    }

    ble_gap_adv_stop();

    int rc = nimble_port_stop();
    if (rc == 0) {
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
