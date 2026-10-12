#pragma once
#include "inspiration_storage.h"
#define INSPIRATION_PAGE_ROWS 3u
typedef enum { IDEA_QUERY, IDEA_PAGE, IDEA_READ, IDEA_SAVE, IDEA_DELETE } inspiration_operation_t;
typedef struct {
    uint32_t token;
    inspiration_operation_t operation;
    unsigned index;
    inspiration_record_t record;
} inspiration_request_t;
typedef struct {
    uint32_t token;
    inspiration_operation_t operation;
    inspiration_status_t status;
    unsigned count, index, rows;
    inspiration_record_t records[INSPIRATION_PAGE_ROWS];
} inspiration_reply_t;
void inspiration_service_init(void);
bool inspiration_service_request(const inspiration_request_t *request);
bool inspiration_service_receive(inspiration_reply_t *reply);
#ifndef ESP_PLATFORM
void inspiration_service_init_at(const char *path);
#endif
