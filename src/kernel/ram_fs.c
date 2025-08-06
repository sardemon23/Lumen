#include "stdio.h"
#include "string.h"
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include "memory.h"
#include "util/sys_log.h"
#define FS_MAX_FILES 16
#define FS_MAX_FILENAME_LEN 32
#define FS_MAX_FILE_SIZE 1024  // maximum size for a file in my EXTREMELY advanced fs
#define FS_STORAGE_SIZE (FS_MAX_FILES * FS_MAX_FILE_SIZE)

typedef struct {
    char name[FS_MAX_FILENAME_LEN];
    uint32_t size;
    uint8_t* data;  
} fs_file_t;

typedef struct {
    fs_file_t files[FS_MAX_FILES];
    uint8_t storage[FS_STORAGE_SIZE];
    uint32_t next_free_offset;
} ramfs_t;

static ramfs_t fs;

void fs_init() {
    memset(&fs, 0, sizeof(fs));
    fs.next_free_offset = 0;
}
int fs_modify_file(const char* name, const char* new_data, uint32_t new_size) {
    if (new_size > FS_MAX_FILE_SIZE) return -1; // too big

    for (int i = 0; i < 10; i++) {
        if (memcmp(fs.files[i].name, name, 6) == 0) {
            memcpy(fs.files[i].data, new_data, new_size);
            fs.files[i].size = new_size;
            return 0; // success
        }
    }
    return -1; // file not found
}


int fs_add_file(const char* name, const char* data, uint32_t size) {
    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (strncmp(fs.files[i].name, name, FS_MAX_FILENAME_LEN) == 0) {
            // Overwrite existing file
            fs.files[i].size = size;
            memcpy(fs.files[i].data, data, size);
            return 0;
        }
    }

    // Add new file if no existing file found
    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (fs.files[i].name[0] == '\0') {
            strncpy(fs.files[i].name, name, FS_MAX_FILENAME_LEN);
            fs.files[i].size = size;
            memcpy(fs.files[i].data, data, size);
            return 0;
        }
    }

    return -1; // no space
}


int fs_read_file(const char* name, uint8_t* buffer, uint32_t max_size) {
    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (strncmp(fs.files[i].name, name, FS_MAX_FILENAME_LEN) == 0) {
            if (max_size < fs.files[i].size) return -1; // buffer trop petit
            memcpy(buffer, fs.files[i].data, fs.files[i].size);
            return fs.files[i].size;
        }
    }
    return -2; 
}

int fs_delete_file(const char* name) {
    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (strncmp(fs.files[i].name, name, FS_MAX_FILENAME_LEN) == 0) {
            fs.files[i].name[0] = '\0';  
            fs.files[i].size = 0;
            fs.files[i].data = NULL;
            log_write(LOG_DELETED,name);
            return 0;
        }
    }
    return -1; // File not found
}
//list all file
void fs_list_files() {
    
    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (fs.files[i].name[0] != '\0') {
            printf("%s\n", fs.files[i].name);
        }
    }
}

// exemple
#ifdef FS_TEST
int main() {
    fs_init();

    const char* hello = "Hello RAMFS!";
    fs_add_file("greet.txt", (const uint8_t*)hello, strlen(hello) + 1);

    uint8_t buffer[64];
    int size = fs_read_file("greet.txt", buffer, sizeof(buffer));
    if (size > 0) {
        printf("Lu depuis RAMFS: %s\n", buffer);
    }
    return 0;
}
#endif
