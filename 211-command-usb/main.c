#include "led.h"
#include "log.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h" 
#include <stdio.h>
#include "device.h"
#include <string.h>
#include "memory.h"

#define LOG_ERR(fmt, ...) printf(fmt, __VA_ARGS__) 

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
// 2. Вынести действие каждой команды в свою функцию
// ==========================================

void cmd_enable(void)
{
    gpio_put(LED_PIN, 1);
    printf("led on\n"); 
}

void cmd_disable(void)
{
    gpio_put(LED_PIN, 0);
    printf("led off\n"); 
}

void cmd_info(void)
{
    printf("project: 211-command-usb\n");
}

void cmd_version(void)
{
    printf("Firmware version: 2.1.1\n");
}

void cmd_ping(void)
{
    printf("pong\n"); 
}

// ==========================================
// 3. Собрать таблицу команд (Скриншот 3)
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
    { "mem_info", mem_info },
};

#define COMMAND_COUNT (sizeof(commands) / sizeof(commands[0]))

// ==========================================
// 4. Заменить цепочку поиском по таблице (Скриншот 4)
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
// Главный цикл обработки ввода
// ==========================================
int main() 
{
    stdio_init_all();
    
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // ИСПРАВЛЕНО: Выделяем нормальный массив символов под строку (на 64 символа)
    char input_buffer[64]; 
    int buf_idx = 0;
    
    while (true) 
    {
        int ch = getchar_timeout_us(0);
        
        if (ch != PICO_ERROR_TIMEOUT) 
        {
            putchar(ch); // Обязательный эхо-вывод принятого символа

            // Если поймали конец строки (\n или \r)
            if (ch == '\n' || ch == '\r') 
            {
                if (buf_idx > 0) 
                {
                    input_buffer[buf_idx] = '\0'; // Закрываем C-строку
                    
                    // Передаем чистую команду на разбор
                    handle_command(input_buffer); 
                    
                    buf_idx = 0; // Сбрасываем индекс для следующей команды
                }
            } 
            // Накапливаем только обычные печатные символы букв команд
            else if (ch >= 32 && ch <= 126) 
            {
                if (buf_idx < sizeof(input_buffer) - 1) 
                {
                    input_buffer[buf_idx++] = (char)ch;
                }
            }
        }
        
        sleep_ms(10);
    }
}