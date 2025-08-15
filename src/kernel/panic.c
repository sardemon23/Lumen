#include "lib/stdio.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "colors.h"
#define VGA_WIDTH   80
#define VGA_HEIGHT  25
#define VGA_MEMORY  0xB8000 

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
//uint16_t* terminal_buffer = (uint16_t*)VGA_MEMORY;

void terminal_setcolor(uint8_t color) 
{
	terminal_color = color;
}
static bool panicking = false;
void panic(const char* message) 
{
	if (panicking){
		__asm__ volatile ("hlt");
	}
    panicking = true;
    clrscr_with_color(vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLUE));

    printf("\n\n\n"); 
    printf("*******************************KERNEL PANIC!************************************\n\n");
    printf("%s\n\n", message);
    printf("Critical Error! System halted.\n");

    __asm__ volatile ("ret");
    __asm__ volatile ("cli");   // desactivate interrupts
    while (1) {
        __asm__ volatile ("hlt"); //to says "STOP" to the CPU
    }

}
