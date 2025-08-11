#pragma once
#include <stdint.h>

typedef struct task {
    uint32_t esp;
    uint32_t ebp;
    uint32_t eip;
    struct task* next;
    uint8_t* stack;
} task_t;

void init_tasking();
void task_create(void (*func)());
void scheduler_tick();
void switch_task();
void task_tick(void);
