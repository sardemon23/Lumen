#pragma once
#include "stdint.h"
#include "../util/types.h"
#include "stddef.h"
typedef struct mem_block {
    size_t size;
    struct mem_block* next;
    int free;
} mem_block_t;

void* memcpy(void* dst, const void* src, size_t num);
void* memset(void* ptr, int value, size_t num);
int memcmp(const void* ptr1, const void* ptr2, size_t num);

void heap_init(void);
void* kmalloc(size_t size);
void kfree(void* ptr);

void print_heap_status();