#pragma once


#include <stdint.h>

void timer_initialize(uint32_t frequency);
uint32_t timer_tick();
void timer_wait(uint32_t tick);
int uptime_seconds();
