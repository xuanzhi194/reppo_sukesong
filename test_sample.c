/*
 * test_sample.c
 * 
 * This is a simple test file for the ESP-IDF project.
 * It contains basic utility functions for demonstration purposes.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define TEST_VERSION "1.0.0"

/**
 * @brief Simple addition function for testing
 */
int test_add(int a, int b)
{
    return a + b;
}

/**
 * @brief Simple string length calculator
 */
size_t test_strlen(const char *str)
{
    size_t len = 0;
    while (str && str[len] != '\0') {
        len++;
    }
    return len;
}

/**
 * @brief Test function to print a greeting
 */
void test_hello(void)
{
    printf("Hello from test_sample.c! Version: %s\n", TEST_VERSION);
}

/**
 * @brief Simple GPIO pin state test (simulated)
 */
bool test_gpio_get_level(uint32_t pin_num)
{
    // Simulated: returns true for even pins, false for odd pins
    return (pin_num % 2 == 0);
}

#ifdef TEST_STANDALONE
int main(void)
{
    test_hello();
    printf("2 + 3 = %d\n", test_add(2, 3));
    printf("String length of 'hello' = %zu\n", test_strlen("hello"));
    printf("GPIO 4 level: %s\n", test_gpio_get_level(4) ? "HIGH" : "LOW");
    return 0;
}
#endif
