#include "timer.h"
#include "../lib/stdio.h"
#include "../arch/i686/irq.h"
#include "../arch/i686/io.h"
#include <stdint.h>
#include "../multitask/task.h"
static uint32_t tick = 0;
static const int TICKS_PER_SECOND = 100; 

uint32_t timer_ticks() {
    return tick;
}

void timer_callback(Registers* regs) {
    tick++;
    
}

void timer_initialize(uint32_t frequency) {
    i686_IRQ_RegisterHandler(0, scheduler_tick);

    uint32_t divisor = 1193180 / frequency;

    i686_outb(0x43, 0x36);
    i686_outb(0x40, divisor & 0xFF);       // Byte down
    i686_outb(0x40, (divisor >> 8) & 0xFF); // Byte Up
}

void timer_wait(uint32_t ticks_to_wait) {
    uint32_t target = tick + ticks_to_wait;
    while (tick < target);
}

int uptime_seconds() {
    return timer_ticks() / TICKS_PER_SECOND;
}

void sleep_ms(uint32_t ms) {
    __asm__ volatile("sti");
    uint32_t ticks_to_wait = (ms * TICKS_PER_SECOND) / 1000;
    timer_wait(ticks_to_wait);
}
