#include "button-task/button-task.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"


static button_state_t button_state;

static const uint64_t DEBOUNCE_US = 20000; // 20 мс
static const uint BUTTON_PIN = 15;
static uint64_t last_pressed_us;
static uint64_t presses;

void button_task_init(void)
{
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);
    button_state = BUTTON_STATE_RELEASED;
    presses = 0;
}

void button_task_handle(void)
{
    
    bool pin_high = gpio_get(BUTTON_PIN); 
    uint64_t now_us = time_us_64();
    
    switch (button_state)
    {
        case BUTTON_STATE_RELEASED:
            if (!pin_high) 
            {
                last_pressed_us = now_us; 
                button_state = BUTTON_STATE_PRESSING;
            }
            break;

        case BUTTON_STATE_PRESSING:
            
            if (now_us - last_pressed_us >= DEBOUNCE_US)
            {
                if (!pin_high) 
                {
                    button_state = BUTTON_STATE_PRESSED;
                    presses++;
                    led_task_next_state(); 
                }
                else 
                {
                    button_state = BUTTON_STATE_RELEASED; 
                }
            }    
            break;
            
        case BUTTON_STATE_PRESSED:
            if (pin_high) 
            {
                last_pressed_us = now_us; 
                button_state = BUTTON_STATE_RELEASING;              
            }
            break;

        case BUTTON_STATE_RELEASING:
            
            if (now_us - last_pressed_us >= DEBOUNCE_US)
            {
                if (pin_high) 
                {
                    button_state = BUTTON_STATE_RELEASED;
                }
                else 
                {
                    button_state = BUTTON_STATE_PRESSED; 
                }
            }
            break;
            
        default:
            button_state = BUTTON_STATE_RELEASED;
            break;
    }
}


void led_task_next_state(void)
{
    
}

bool button_task_is_pressed(void)
{
   
    return (button_state == BUTTON_STATE_PRESSED);
}

uint64_t button_task_get_press_count(void)
{
    return presses; 
}
