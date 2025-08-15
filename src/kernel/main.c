#include <stdint.h>

#include "module/module.h"
#include "colors.h"
#include "keyboard.h"
#include "shell.h"
#include "lib/stdio.h"
#include "memory/memory.h"
#include "hal/hal.h"
#include "arch/i686/irq.h"
#include "sys.h"
#include "user.h"
#include "util/timer.h"
#include "panic.h"
#include "multitask/task.h"
extern uint8_t __bss_start;
extern uint8_t __end;


#define VGA_MEMORY ((volatile uint16_t*)0xB8000)
#define VGA_WIDTH 80
#define VGA_HEIGHT 25



void crash_me();
// a timer for checking interrupts
void timer(Registers* regs)
{
    static int t = 0;
    t++;
    if (t >=100000){
        t = 0;
        printf(".");
    }

}
#define VGA_MEMORY ((volatile uint16_t*)0xB8000)
#define VGA_WIDTH 80
#define VGA_HEIGHT 25





void clear_screen(uint8_t color) {
    volatile uint16_t* vga_buffer = VGA_MEMORY;
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vga_buffer[i] = vga_entry(' ', color);
    }
}

void print_char_at(char c, int x, int y, uint8_t color) {
    volatile uint16_t* vga_buffer = VGA_MEMORY;
    vga_buffer[y * VGA_WIDTH + x] = vga_entry(c, color);
}

void print_lumen_test() {
    clear_screen(vga_entry_color(VGA_COLOR_BLACK, VGA_COLOR_BLACK));
    uint8_t color = vga_entry_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    char* lumen = "LUMEN";
    for (int i = 0; lumen[i] != '\0'; i++) {
        print_char_at(lumen[i], i + 10, 5, color);
    }
}
void print_lumen_splash() {
    // Change la couleur en cyan clair sur fond noir
    
    printf(
    "$$\\                                                   \n"
    "$$ |                                                  \n"
    "$$ |     $$\\   $$\\ $$$$$$\\$$$$\\   $$$$$$\\  $$$$$$$\\  \n"
    "$$ |     $$ |  $$ |$$  _$$  _$$\\ $$  __$$\\ $$  __$$\\ \n"
    "$$ |     $$ |  $$ |$$ / $$ / $$ |$$$$$$$$ |$$ |  $$ |\n"
    "$$ |     $$ |  $$ |$$ | $$ | $$ |$$   ____|$$ |  $$ |\n"
    "$$$$$$$$\\\\$$$$$$  |$$ | $$ | $$ |\\$$$$$$$\\ $$ |  $$ |\n"
    "\\________|\\______/ \\__| \\__| \\__| \\_______|\\__|  \\__|\n"
    "                                                    \n"
    "                                                    \n"
    "                                                    \n"
    );
    printf("Embrace Lumen, a little bit of light into the darkness of today's technologie\n");

    
}





void __attribute__((section(".entry"))) start(uint16_t bootDrive)
{
    timer_initialize(100); //100Hz timer
    memset(&__bss_start, 0, (&__end) - (&__bss_start));
    //Turns on all the interrupt thing
    HAL_Initialize();
    i686_IRQ_RegisterHandler(0, timer);

    clrscr();
    init_users();
    print_lumen_splash();
    
    keyboard_initialize();

    //i686_IRQ_RegisterHandler(0, timer);
    uptime_seconds();
    shell_init();
    terminal_setcolor(vga_entry_color(VGA_COLOR_BLACK, VGA_COLOR_WHITE));

    // you know that i passed 1 hours figuring how to f*ck it kernel panic at start....
    // Turns out it was this crash_me() call (commented out now)
    //crash_me();
    

end:
    /*
    Infinite loooop to keep the kernel alive
    because of the giga chad CPU constantly hungry
    to eat binary code
    */
    for (;;);
}
