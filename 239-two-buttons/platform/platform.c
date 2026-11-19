#include "platform.h"
#include "pico/stdlib.h"

#define LED_PIN 25
#define BUTTON_0_PIN 15
#define BUTTON_1_PIN 14

static const uint32_t button_pin[] = {BUTTON_0_PIN, BUTTON_1_PIN};

void platform_init(void)
{
    stdio_init_all();

    gpio_init(BUTTON_0_PIN);
    gpio_set_dir(BUTTON_0_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_0_PIN);

    gpio_init(BUTTON_1_PIN);
    gpio_set_dir(BUTTON_1_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_1_PIN);

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    platform_led_set(true);
}

void platform_led_set(bool on)
{
    gpio_put(LED_PIN, on);
}

bool platform_led_get(void)
{
    return !gpio_get(LED_PIN);
}

bool platform_button_read(uint32_t btn)
{
    if (btn >= 2)
    {
        return false;
    }
    return !gpio_get(button_pin[btn]);
}