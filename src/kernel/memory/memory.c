#include <stddef.h>
#include <stdint.h>
#include "../lib/stdio.h"
#include "memory.h"
// --- memcpy amélioré ---
void* memcpy(void* dst, const void* src, size_t num) {
    uint8_t* d = (uint8_t*)dst;
    const uint8_t* s = (const uint8_t*)src;

    // On copie octet par octet
    for (size_t i = 0; i < num; i++) {
        d[i] = s[i];
    }
    return dst;
}

// --- memset amélioré ---
void* memset(void* ptr, int value, size_t num) {
    uint8_t* p = (uint8_t*)ptr;

    for (size_t i = 0; i < num; i++) {
        p[i] = (uint8_t)value;
    }
    return ptr;
}

// --- memcmp---
// return 0 if equal
int memcmp(const void* ptr1, const void* ptr2, size_t num) {
    const uint8_t* p1 = (const uint8_t*)ptr1;
    const uint8_t* p2 = (const uint8_t*)ptr2;

    for (size_t i = 0; i < num; i++) {
        if (p1[i] != p2[i]) {
            return (int)p1[i] - (int)p2[i];
        }
    }
    return 0;
}

#define BLOCK_SIZE sizeof(mem_block_t)

// Exemple de heap statique (1 Mo)
#define HEAP_SIZE (1024 * 1024)
static uint8_t heap[HEAP_SIZE];
static mem_block_t* free_list = NULL;

// Initialisation of the heap, to call one time
void heap_init(void) {
    if (free_list != NULL) return; // déjà initialisé
    free_list = (mem_block_t*)heap;
    free_list->size = HEAP_SIZE - BLOCK_SIZE;
    free_list->next = NULL;
    free_list->free = 1;
}


// search a block of free memory
static mem_block_t* find_free_block(size_t size) {
    mem_block_t* current = free_list;

    while (current) {
        if (current->free && current->size >= size) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// cut a block if enough big
static void split_block(mem_block_t* block, size_t size) {
    if (block->size >= size + BLOCK_SIZE + 8) {
        mem_block_t* new_block = (mem_block_t*)((uint8_t*)block + BLOCK_SIZE + size);
        new_block->size = block->size - size - BLOCK_SIZE;
        new_block->free = 1;
        new_block->next = block->next;

        block->size = size;
        block->next = new_block;
    }
}

// kmalloc 
void* kmalloc(size_t size) {
    if (size == 0) {
        return NULL;
    }
    if (size > HEAP_SIZE) {
        printf("Erreur: tentative d'allocation trop grande (%d octets)\n", size);
        return NULL;
    }
    mem_block_t* block = find_free_block(size);
    if (!block) {
        printf("Erreur: pas assez de mémoire (%d octets demandés)\n", size);
        return NULL;
    }


    split_block(block, size);
    block->free = 0;

    return (void*)(block + 1);
}

// kfree free a block of memory
void kfree(void* ptr) {
    if (!ptr) {
        return;
    }

    mem_block_t* block = (mem_block_t*)ptr - 1;
    block->free = 1;

    // merge with the next block if possible 
    if (block->next && block->next->free) {
        block->size += BLOCK_SIZE + block->next->size;
        block->next = block->next->next;
    }
    // merge with the previous block if possible
    mem_block_t* current = free_list;
    while (current && current->next != block) {
        current = current->next;
    }
    if (current && current->free) {
        current->size += BLOCK_SIZE + block->size;
        current->next = block->next;
}

}
void print_heap_status() {
    mem_block_t* current = free_list;
    int i = 0;

    printf("=== Heap Status ===\n");
    while (current) {
        printf("Bloc %d: %s, taille: %u octets\n", i, current->free ? "LIBRE" : "UTILISÉ", current->size);
        current = current->next;
        i++;
    }
    printf("===================\n");
}
