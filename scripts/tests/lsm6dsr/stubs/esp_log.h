#ifndef LSM6DSR_TEST_LOG_H
#define LSM6DSR_TEST_LOG_H
static inline void lsm6dsr_test_log(const char *tag, const char *format, ...)
{
    (void)tag;
    (void)format;
}
#define ESP_LOGI(...) lsm6dsr_test_log(__VA_ARGS__)
#define ESP_LOGW(...) lsm6dsr_test_log(__VA_ARGS__)
#define ESP_LOGE(...) lsm6dsr_test_log(__VA_ARGS__)
#endif
