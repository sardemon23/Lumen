#pragma once
#include <stdint.h>
#include "arch/i686/irq.h"
#include "stdbool.h"

#define INPUT_BUFFER_SIZE 256
extern char input_buffer[INPUT_BUFFER_SIZE];

extern int input_index;
extern bool is_keyboard_alive;
void keyboard_initialize();
void keyboard_handler(Registers* regs);
extern unsigned char scancode_to_ascii_azerty[256];
void get_string_scancode(char *buf, int max_len);
void wait_for_key_release(uint8_t scancode);
void redraw_input_line_kscanf(const char* buf) ;