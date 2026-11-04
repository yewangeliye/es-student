#include "led-task.h"

#include "pico/stdlib.h"


static led_state_t led_state;
static uint64_t half_period_us;
static uint64_t last_toggle_us;
static const uint32_t DEFAULT_LED_BLINK_PERIOD_MS=1000;

bool period_elapsed(uint64_t *last_us, uint64_t period_us);


static void led_set(bool on)
{
    gpio_put(PICO_DEFAULT_LED_PIN, on);
}

bool led_task_set_period_ms(uint32_t period_ms)
{
    
    if (period_ms == 0) {
        return false;
    }

    half_period_us = (uint64_t)period_ms * 1000 / 2;
    return true;
}

void led_task_init(void)
{
	gpio_init(PICO_DEFAULT_LED_PIN);
	gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

	led_task_set_period_ms(DEFAULT_LED_BLINK_PERIOD_MS);
    led_task_set_state(LED_STATE_BLINK);
}

void led_task_set_state(led_state_t state)
{
    led_state = state;
	switch (led_state)
	{
		case LED_STATE_OFF:
		{
			led_set(false);
			break;
		}
		case LED_STATE_ON:
		{
			led_set(true);
			break;
		}
		case LED_STATE_BLINK:
		{
			last_toggle_us = 0;
			break;
		}


	
	}
}

led_state_t led_task_get_state(void)
{
	return led_state;
}

void led_task_handle(void)
{
    switch (led_state)
    {	
		case LED_STATE_OFF:
		case LED_STATE_ON:
			break;
	    
		case LED_STATE_BLINK:
			if (period_elapsed(&last_toggle_us, half_period_us)) 
			{
				gpio_put(PICO_DEFAULT_LED_PIN, !gpio_get(PICO_DEFAULT_LED_PIN));
			}
			break;
    }
}

uint32_t led_task_get_period_ms(void)
{
	return (uint32_t)(half_period_us / 500);
}

bool period_elapsed(uint64_t *last_us, uint64_t period_us)
{
    uint64_t now_us = time_us_64();

    
    if (now_us - *last_us >= period_us) {


    
    *last_us += period_us;

    
    if (now_us - *last_us >= period_us) {
        *last_us = now_us;
    }

    return true;
}
        return false;
}