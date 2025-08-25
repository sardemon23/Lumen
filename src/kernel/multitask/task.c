#include "task.h"
#include "../arch/i686/irq.h"
#include "../memory/memory.h"
#include "../lib/stdio.h"

#define STACK_SIZE 4096
#define KERNEL_CS 0x08  

extern void task_switch_asm(uint32_t* esp, uint32_t* ebp, uint32_t eip);

static task_t* current_task = NULL;

void task_create(void (*func)()) {
    task_t* new_task = kmalloc(sizeof(task_t));

    new_task->stack = kmalloc(STACK_SIZE);
    uint32_t* stack_top = (uint32_t*)(new_task->stack + STACK_SIZE);

    
    *(--stack_top) = (uint32_t)func;      // EIP
    *(--stack_top) = KERNEL_CS;           // CS
    *(--stack_top) = 0x202;               // EFLAGS (IF=1)

    
    *(--stack_top) = 0; // EAX
    *(--stack_top) = 0; // ECX
    *(--stack_top) = 0; // EDX
    *(--stack_top) = 0; // EBX
    *(--stack_top) = 0; // ESP (fake)
    *(--stack_top) = 0; // EBP
    *(--stack_top) = 0; // ESI
    *(--stack_top) = 0; // EDI

    new_task->esp = (uint32_t)stack_top;
    new_task->ebp = 0;
    new_task->eip = (uint32_t)func;

    
    if (!current_task) {
        current_task = new_task;
        new_task->next = new_task;
    } else {
        new_task->next = current_task->next;
        current_task->next = new_task;
    }
}

void init_tasking() {
    current_task = NULL;
}

void switch_task() {
    if (!current_task || !current_task->next) return;

    current_task = current_task->next;
    task_switch_asm(&current_task->esp, &current_task->ebp, current_task->eip);
}

void scheduler_tick() {
    switch_task();
}
