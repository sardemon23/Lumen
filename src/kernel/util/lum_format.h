// lumn_format.h
#pragma once
#include "../lib/ctype.h"
typedef struct {
    char magic[3];       // "LUMN"
    uint32_t entry_point; // Offset of the file
    uint32_t size;        // Size of the code
    // + other info
} __attribute__((packed)) lumn_header_t;
