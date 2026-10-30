#include "inspiration_service.h"
#include <string.h>
#ifdef ESP_PLATFORM
#include "esp_attr.h"
#include "esp_spiffs.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#define IDEA_RAM EXT_RAM_BSS_ATTR
static QueueHandle_t s_requests, s_replies;
static StaticQueue_t s_request_control, s_reply_control;
static QueueHandle_t s_summary_replies;
static StaticQueue_t s_summary_control;
static uint8_t s_summary_buffer[sizeof(inspiration_reply_t)] IDEA_RAM;
static uint8_t s_request_buffer[sizeof(inspiration_request_t)] IDEA_RAM;
static uint8_t s_reply_buffer[sizeof(inspiration_reply_t)] IDEA_RAM;
#else
#define IDEA_RAM
static bool s_pending, s_reply_ready;
static bool s_summary_ready;
static inspiration_reply_t s_summary_reply;
#endif
static inspiration_request_t s_request IDEA_RAM;
static inspiration_reply_t s_reply IDEA_RAM;
static inspiration_status_t s_init_status;

static void process(void)
{
    memset(&s_reply, 0, sizeof(s_reply));
    s_reply.token = s_request.token;
    s_reply.operation = s_request.operation;
    s_reply.index = s_request.index;
    s_reply.status = s_init_status;
    if (s_init_status != INSPIRATION_OK) return;
    s_reply.status = INSPIRATION_OK;
    if (s_request.operation == IDEA_SAVE) {
#ifdef ESP_PLATFORM
        size_t total = 0, used = 0;
        if (esp_spiffs_info("storage", &total, &used) != ESP_OK) s_reply.status = INSPIRATION_IO;
        /* Leave GC and ebook/todo space. Never consume the last filesystem blocks. */
        else if (used > total || total - used < total / 4 + INSPIRATION_RECORD_BYTES)
            s_reply.status = INSPIRATION_NO_SPACE;
#endif
        if (s_reply.status == INSPIRATION_OK) s_reply.status = inspiration_storage_add(&s_request.record);
    } else if (s_request.operation == IDEA_DELETE) {
        s_reply.status = inspiration_storage_delete(s_request.index);
    } else if (s_request.operation == IDEA_READ) {
        s_reply.status = inspiration_storage_read(s_request.index, &s_reply.records[0]);
        s_reply.rows = s_reply.status == INSPIRATION_OK;
    } else if (s_request.operation == IDEA_PAGE || s_request.operation == IDEA_SUMMARY) {
        unsigned count = inspiration_storage_count();
        for (unsigned i = 0; i < INSPIRATION_PAGE_ROWS && s_request.index + i < count; ++i) {
            s_reply.status = inspiration_storage_read(s_request.index + i, &s_reply.records[i]);
            if (s_reply.status != INSPIRATION_OK) break;
            ++s_reply.rows;
        }
    }
    s_reply.count = inspiration_storage_count();
}

#ifdef ESP_PLATFORM
static void worker(void *unused)
{
    (void)unused;
    s_init_status = inspiration_storage_init("/spiffs/inspirations.dat");
    for (;;) {
        if (xQueueReceive(s_requests, &s_request, portMAX_DELAY) != pdTRUE) continue;
        process();
        if (s_request.operation == IDEA_SUMMARY) xQueueOverwrite(s_summary_replies, &s_reply);
        else xQueueSend(s_replies, &s_reply, portMAX_DELAY);
    }
}
void inspiration_service_init(void)
{
    s_requests = xQueueCreateStatic(1, sizeof(inspiration_request_t), s_request_buffer, &s_request_control);
    s_replies = xQueueCreateStatic(1, sizeof(inspiration_reply_t), s_reply_buffer, &s_reply_control);
    s_summary_replies = xQueueCreateStatic(1, sizeof(inspiration_reply_t), s_summary_buffer, &s_summary_control);
    if (!s_requests || !s_replies || !s_summary_replies || xTaskCreatePinnedToCore(worker, "idea_store", 4096, NULL, 4, NULL, 0) != pdPASS) {
        s_requests = s_replies = NULL;
        s_summary_replies = NULL;
        ESP_LOGE("IDEA", "Storage worker unavailable");
    }
}
bool inspiration_service_request(const inspiration_request_t *r)
{ return s_requests && xQueueSend(s_requests, r, 0) == pdTRUE; }
bool inspiration_service_receive(inspiration_reply_t *r)
{ return s_replies && xQueueReceive(s_replies, r, 0) == pdTRUE; }
bool inspiration_service_summary_receive(inspiration_reply_t *r)
{ return s_summary_replies && xQueueReceive(s_summary_replies, r, 0) == pdTRUE; }
#else
void inspiration_service_init_at(const char *path)
{ s_pending = s_reply_ready = s_summary_ready = false; s_init_status = inspiration_storage_init(path); }
void inspiration_service_init(void) { inspiration_service_init_at("/tmp/reppo-inspirations.dat"); }
bool inspiration_service_request(const inspiration_request_t *r)
{ if (s_pending || s_reply_ready) return false; s_request = *r; s_pending = true; return true; }
static void host_process(void)
{ if (s_pending) { process(); s_pending = false;
    if (s_request.operation == IDEA_SUMMARY) { s_summary_reply = s_reply; s_summary_ready = true; }
    else s_reply_ready = true; } }
bool inspiration_service_receive(inspiration_reply_t *r)
{ host_process();
  if (!s_reply_ready) return false;
  *r = s_reply; s_reply_ready = false; return true; }
bool inspiration_service_summary_receive(inspiration_reply_t *r)
{ host_process(); if (!s_summary_ready) return false;
  *r = s_summary_reply; s_summary_ready = false; return true; }
#endif

bool inspiration_service_summary_request(uint32_t token)
{
    inspiration_request_t r = {.operation = IDEA_SUMMARY, .token = token};
    return inspiration_service_request(&r);
}
