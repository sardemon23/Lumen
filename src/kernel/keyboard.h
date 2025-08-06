#pragma once

#include <stdint.h>
#include "arch/i686/irq.h"

void keyboard_initialize();
void keyboard_handler(Registers* regs);