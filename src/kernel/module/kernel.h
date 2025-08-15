// kernel.h
#ifndef KERNEL_H
#define KERNEL_H

// Lib C 
#include "../lib/stdio.h"
#include "../lib/string.h"

// Struct and type
#include "../util/types.h"
#include "../user.h"
#include "../sys.h"



// Hardware abstraction layer
#include "../hal/hal.h"
#include "../arch/i686/gdt.h"
#include "../arch/i686/idt.h"
#include "../arch/i686/io.h"

// Kernel subsystems
#include "../multitask/task.h"
#include "../fs/ram_fs.h"
#include "../keyboard.h"
#include "../panic.h"
#include "module.h"

#endif
