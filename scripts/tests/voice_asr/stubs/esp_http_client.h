#pragma once
#include <stdbool.h>
#include <stdint.h>
typedef void *esp_http_client_handle_t;
typedef struct {
    const char *url;
    int method;
    int timeout_ms;
    int (*crt_bundle_attach)(void *);
    int buffer_size;
    int buffer_size_tx;
} esp_http_client_config_t;
#define HTTP_METHOD_POST 1
#define ESP_OK 0
esp_http_client_handle_t esp_http_client_init(const esp_http_client_config_t *cfg);
int esp_http_client_set_header(esp_http_client_handle_t client, const char *key, const char *value);
int esp_http_client_open(esp_http_client_handle_t client, int length);
int esp_http_client_write(esp_http_client_handle_t client, const char *data, int length);
int64_t esp_http_client_fetch_headers(esp_http_client_handle_t client);
int esp_http_client_get_status_code(esp_http_client_handle_t client);
int esp_http_client_read(esp_http_client_handle_t client, char *data, int length);
bool esp_http_client_is_complete_data_received(esp_http_client_handle_t client);
int esp_http_client_close(esp_http_client_handle_t client);
int esp_http_client_cleanup(esp_http_client_handle_t client);
