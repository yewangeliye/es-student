#include "led.h"
#include "log.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h" 
#include <stdio.h>
#include "device.h"
#include <string.h>

#define LED_PIN 25

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

typedef void (*command_handler_t)(void);

// ==========================================
// 2. Функции-обработчики для каждой команды
// ==========================================

void cmd_enable(void)
{
    gpio_put(LED_PIN, 1);
    printf("LED enabled\n");
}

void cmd_disable(void)
{
    gpio_put(LED_PIN, 0);
    printf("LED disabled\n");
}

void cmd_info(void)
{
    printf("Device: Raspberry Pi Pico USB Command Controller\n");
}

void cmd_version(void)
{
    printf("Firmware version: 2.1.1\n");
}

// 5. Добавлена новая команда ping
void cmd_ping(void)
{
    printf("pong\n");
}

// ==========================================
// 3. Собрать таблицу команд
// ==========================================
struct command_t 
{
    const char *name;
    command_handler_t handler;
};

const struct command_t commands[] = {
    { "enable",  cmd_enable },
    { "disable", cmd_disable },
    { "info",    cmd_info },
    { "version", cmd_version },
    { "ping",    cmd_ping }, 
};

// Исправлено: корректный подсчет размера массива
#define COMMAND_COUNT (sizeof(commands) / sizeof(commands[0]))

// ==========================================
// 4. Функция поиска команды по таблице
// ==========================================
void handle_command(const char *command)
{
    for (uint i = 0; i < COMMAND_COUNT; i++)
    {
        if (strcmp(command, commands[i].name) == 0)
        {
            if (commands[i].handler != NULL)
            {
                commands[i].handler();
            }
            return;
        }
    }
    
    LOG_ERR("unknown command: %s\n", command);
}

// ==========================================
// Главная функция
// ==========================================
int main() 
{
    // Инициализация стандартного ввода-вывода (USB Serial)
    stdio_init_all();
    
    // Инициализация пина светодиода
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    char input_buffer[64];
    int buf_idx = 0;
    
    while (true) 
    {
        // Чтение символов из USB в неблокирующем режиме
        int ch = getchar_timeout_us(0);
        
        if (ch != PICO_ERROR_TIMEOUT) 
        {
            // Если пришел символ перевода строки — команда завершена
            if (ch == '\r' || ch == '\n') 
            {
                if (buf_idx > 0) 
                {
                    input_buffer[buf_idx] = '\0'; // Закрываем строку c-string
                    handle_command(input_buffer); // Отправляем на разбор в таблицу
                    buf_idx = 0;                  // Сбрасываем буфер для новой команды
                }
            } 
            // Защита от переполнения буфера
            else if (buf_idx < sizeof(input_buffer) - 1) 
            {
                input_buffer[buf_idx++] = (char)ch;
            }
        }
        
        sleep_ms(10);
    }
}