#ifndef RX8130_TEST_LOG_H
#define RX8130_TEST_LOG_H
static inline void rx8130_test_log(const char *tag, const char *format, ...)
{
    (void)tag;
    (void)format;
}
#define ESP_LOGI(...) rx8130_test_log(__VA_ARGS__)
#define ESP_LOGW(...) rx8130_test_log(__VA_ARGS__)
#define ESP_LOGE(...) rx8130_test_log(__VA_ARGS__)
#endif
