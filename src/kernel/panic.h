
#ifndef PANIC_H
#define PANIC_H
#include "stdint.h"
#include "colors.h"
void panic(const char* message);

void terminal_setcolor(uint8_t color);


#endif
