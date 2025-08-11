#ifndef RAM_FS_H
#define RAM_FS_H

#include <stdint.h>

#define FS_MAX_FILES 16
#define FS_MAX_FILENAME_LEN 32
#define FS_MAX_FILE_SIZE 1024


void fs_init(void);


int fs_add_file(const char* name, const uint8_t* data, uint32_t size);


int fs_read_file(const char* name, uint8_t* buffer, uint32_t max_size);


int fs_delete_file(const char* name);

void fs_list_files();
#endif // RAM_FS_H
