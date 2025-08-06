#include "timer.h"
#include "stdio.h"
#include "arch/i686/irq.h"
#include "arch/i686/io.h"


static uint32_t tick = 0;

uint32_t timer_ticks() {
    return tick;
}

void timer_callback(Registers* regs) {
    tick++;
    // Here we can add a scheduler
}

void timer_initialize(uint32_t frequency) {
    i686_IRQ_RegisterHandler(0, timer_callback);


    uint32_t divisor = 1193180 / frequency;

    i686_outb(0x43, 0x36);
    i686_outb(0x40, divisor & 0xFF);       // Low byte
    i686_outb(0x40, (divisor >> 8) & 0xFF); // High byte
}

void timer_wait(uint32_t ticks_to_wait) {
    uint32_t target = tick + ticks_to_wait;
    while (tick < target);
}
int uptime_seconds() {
    const int TICKS_PER_SECOND = 100; // must correspond to timers frequency
    return timer_ticks() / TICKS_PER_SECOND;
}