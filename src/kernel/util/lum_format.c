/*
#include "../lib/stdio.h"
#include "../lib/string.h"
#include <stdint.h>
#include <stddef.h>
#include "lum_format.h"
#include "../fs/ram_fs.h" 
#include "../memory/memory.h"
#include "../../bootloader/stage2/fat.h"
#define LUMN_MAX_SIZE 1024
#define EXEC_MEM_ADDR 0x100000  // exemple of reserved executable memory adress

void exec_lum(const uint8_t* buffer, int size) {
    if (size < sizeof(lumn_header_t)) {
        printf("Error: file too small\n");
        return;
    }

    const lumn_header_t* header = (const lumn_header_t*)buffer;

    if (strncmp(header->magic, "LUM", 3) != 0) {
        printf("Error: invalid format\n");
        return;
    }

    void (*entry)() = (void (*)())(buffer + sizeof(lumn_header_t));
    printf("[LUMEN] Executing from buffer...\n");
    entry();
}


int read_lum_file_from_fat(DISK* disk, const char* filename, uint8_t* buffer, int max_size) {
    FAT_File* file = FAT_Open(disk, filename);
    if (!file) {
        printf("File not found: %s\n", filename);
        return -1;
    }

    int bytesRead = FAT_Read(disk, file, max_size, buffer);
    FAT_Close(file);
    return bytesRead;
}

void exec_lum_from_fat(DISK* disk, const char* filename) {
    uint8_t buffer[LUMN_MAX_SIZE];
    int size = read_lum_file_from_fat(disk, filename, buffer, sizeof(buffer));
    if (size <= 0) {
        printf("Erreur lecture fichier %s\n", filename);
        return;
    }
    exec_lum(buffer, size); 
}
*/