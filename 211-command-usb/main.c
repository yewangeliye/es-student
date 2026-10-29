#include "led.h"
#include "log.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h" 
#include <stdio.h>
#include "device.h"
#include <string.h>

#define LINE_SIZE 32

char line[LINE_SIZE];
uint line_length = 0;




const uint DEBOUNCE_MS=20;
const uint BUTTON_PIN = 15;

bool get_button_debounce(uint pin)
{
    bool state = gpio_get(pin);
    sleep_ms(DEBOUNCE_MS);
    return state && gpio_get(pin);
}

void handle_command(const char *command)
{
    if (strcmp(command,"enable")==0)
    {
        led_set(true);
        LOG_INF("led on\n");
    }
    else if (strcmp(command,"disable")==0)
    {
        led_set(false);
        LOG_INF("led off\n");
    }

    else if (strcmp(command,"version")==0)
    {
        log_version();
    }

    else if (strcmp(command, "info") == 0)
    {
        device_info();
    }

    else
    {
        LOG_ERR("unknown command: %s\n", command);
    }
}

void read_line(void)
{
    int symbol = getchar_timeout_us(0);

    if (symbol == PICO_ERROR_TIMEOUT)
    {
        return;
    }

    // Если пришел символ конца строки (\n или \r)
    if (symbol == '\r' || symbol == '\n')
    {
        if (line_length > 0)
        {
            line[line_length] = '\0';

            // 1. Скрипт Питона ждет строгое эхо всей команды одной строкой:
            printf("%s\n", line);
            fflush(stdout); // Моментально отправляем эхо в порт

            // 2. Вызываем обработчик (он напечатает логи inf/err или паспорт)
            handle_command(line);
            fflush(stdout); // Моментально отправляем ответы команды в порт

            line_length = 0;
        }
        return;
    }

    // Просто собираем символы в буфер БЕЗ putchar(symbol)
    if (line_length + 1 < LINE_SIZE)
    {
        line[line_length] = (char)symbol;
        line_length = line_length + 1;
    }
}

int main()
{
    stdio_init_all();

    led_init();

    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN,GPIO_IN);
    gpio_pull_up(BUTTON_PIN);

    bool previous = true; 
    bool current  = true;

    char command[32];
    int command_pos=0;

    while (1)
    {
        bool current = get_button_debounce(BUTTON_PIN); 

        if (previous == true && current == false)
        {
            led_toggle();
            LOG_INF("led %s\n", led_is_on() ? "on" : "off");
        }

        previous=current;

        int c = getchar_timeout_us(0);

        if (c == PICO_ERROR_TIMEOUT)
        {
            continue;
        }
        if (c == '\r' || c == '\n')
        {
            if(command_pos > 0)
            {
                command[command_pos]='\0';
                LOG_DBG("got %s\n", c);
                handle_command(command);

                command_pos = 0;

            }
    }
    else if(command_pos < sizeof(command)-1)
    {
        command[command_pos++]=(char)c;
    }
}
}