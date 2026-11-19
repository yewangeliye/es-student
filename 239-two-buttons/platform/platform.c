#include "platform.h"
#include "pico/stdlib.h"

#define LED_PIN 25
#define BUTTON_1_PIN 10
#define BUTTON_2_PIN 14

static const uint32_t button_pins[2] = {BUTTON_1_PIN, BUTTON_2_PIN};

void platform_init(void)
{
    stdio_init_all();

    for (uint32_t i = 0; i < 2; i++)
    {
        gpio_init(button_pins[i]);
        gpio_set_dir(button_pins[i], GPIO_IN);
        gpio_pull_up(button_pins[i]);
    }

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
    return !gpio_get(button_pins[btn]);
}