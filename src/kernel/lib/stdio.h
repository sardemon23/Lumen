#pragma once
#include <stdint.h>
#include "colors.h"
#include "../keyboard.h"
void clrscr();
void putc(char c);
void puts(const char* str);
void printf(const char* fmt, ...);

void print_buffer(const char* msg, const void* buffer, uint32_t count);
void clrscr_with_color(uint8_t color);
static inline uint16_t vga_entry(unsigned char uc, uint8_t color) 
{
	return (uint16_t) uc | (uint16_t) color << 8;
}
static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
    return fg | (bg << 4);
}



extern char input_buffer[INPUT_BUFFER_SIZE];
extern void redraw_input_line(void);


// La fonction safe_printf complète
void safe_printf(const char* fmt, ...) ;

