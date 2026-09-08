#pragma once

void test_esp_log(const char *tag, const char *format, ...);

#define ESP_LOGE(...) test_esp_log(__VA_ARGS__)
#define ESP_LOGW(...) test_esp_log(__VA_ARGS__)
#define ESP_LOGI(...) test_esp_log(__VA_ARGS__)
#define ESP_LOGD(...) test_esp_log(__VA_ARGS__)
