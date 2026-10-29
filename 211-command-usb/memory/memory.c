#include <stdio.h>
#include "memory.h"
#include "pico/stdlib.h"
#include "hardware/regs/addressmap.h"

extern char __flash_binary_start;
extern char __flash_binary_end;
extern char __boot2_start__;
extern char __boot2_end__;
extern char __etext;
extern char __data_start__;
extern char __data_end__;
extern char __bss_start__;
extern char __bss_end__;
extern char __HeapLimit;
extern char __StackBottom;
extern char __StackTop;

static void row(const char *name, uintptr_t start, uintptr_t end)
{
    printf("%-10s 0x%08x 0x%08x %8u\n",
           name, (unsigned)start, (unsigned)end, (unsigned)(end - start));
}

void mem_info(void)
{
    printf("%-10s %-10s %-10s %-8s\n", "area", "start", "end", "size");

    uintptr_t flash_start = XIP_BASE;
    uintptr_t flash_end   = XIP_BASE + PICO_FLASH_SIZE_BYTES;
    uintptr_t sram_start  = SRAM_BASE;
    uintptr_t sram_end    = SRAM_END;
    uintptr_t rom_start   = ROM_BASE;
    uintptr_t rom_end     = ROM_BASE + 16 * 1024;

    row("flash", flash_start, flash_end);
    row("sram",  sram_start,  sram_end);
    row("rom",   rom_start,   rom_end);

    uintptr_t img_start        = (uintptr_t)&__flash_binary_start;
    uintptr_t img_end          = (uintptr_t)&__flash_binary_end;
    uintptr_t b2_start         = (uintptr_t)&__boot2_start__;
    uintptr_t b2_end           = (uintptr_t)&__boot2_end__;
    uintptr_t txt_start        = (uintptr_t)&__boot2_end__;
    uintptr_t txt_end          = (uintptr_t)&__etext;
    uintptr_t dt_flash_start   = (uintptr_t)&__etext;
    uintptr_t dt_flash_end     = (uintptr_t)&__etext + ((uintptr_t)&__data_end__ - (uintptr_t)&__data_start__);
    uintptr_t dt_ram_start     = (uintptr_t)&__data_start__;
    uintptr_t dt_ram_end       = (uintptr_t)&__data_end__;
    uintptr_t bss_start        = (uintptr_t)&__bss_start__;
    uintptr_t bss_end          = (uintptr_t)&__bss_end__;
    uintptr_t heap_start       = (uintptr_t)&__bss_end__;
    uintptr_t heap_end         = (uintptr_t)&__HeapLimit;
    uintptr_t stack_start      = (uintptr_t)&__StackBottom;
    uintptr_t stack_end        = (uintptr_t)&__StackTop;

    row("image",      img_start,      img_end);
    row("free",       img_end,        flash_end);
    row("boot2",      b2_start,       b2_end);
    row("text",       txt_start,      txt_end);
    row("data flash", dt_flash_start, dt_flash_end);
    row("data ram",   dt_ram_start,   dt_ram_end);
    row("bss",        bss_start,      bss_end);
    row("heap",       heap_start,     heap_end);
    row("stack",      stack_start,    stack_end);

    unsigned size_img      = img_end - img_start;
    unsigned size_boot2    = b2_end - b2_start;
    unsigned size_text     = txt_end - txt_start;
    unsigned size_dt_ram   = dt_ram_end - dt_ram_start;
    unsigned size_free     = flash_end - img_end;
    unsigned size_flash    = flash_end - flash_start;
    unsigned size_bss      = bss_end - bss_start;
    unsigned size_heap     = heap_end - heap_start;
    unsigned size_stack    = stack_end - stack_start;

    unsigned ram_used      = size_dt_ram + size_bss;
    unsigned ram_free      = size_heap + size_stack;

    printf("\ntotal\n");
    printf("    flash image  %5u = boot2 %u + text %u + data %u\n", 
           size_img, size_boot2, size_text, size_dt_ram);
    printf("    flash free   %u of %u\n", size_free, size_flash);
    printf("    ram used     %5u = data %u + bss %u\n", 
           ram_used, size_dt_ram, size_bss);
    printf("    ram free     %5u for heap and %u for stack\n", 
           ram_free, size_stack);
}