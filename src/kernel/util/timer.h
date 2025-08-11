#pragma once


#include "../lib/ctype.h"


void timer_initialize(uint32_t frequency);
uint32_t timer_tick();
void timer_wait(uint32_t tick);
int uptime_seconds();
void sleep_ms(uint32_t ms);

