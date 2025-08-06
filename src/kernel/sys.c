#include "sys.h"
#include <stdio.h>
#include "ram_fs.h"
#include "memory.h"
#include "string.h"
#include "user.h"


// some variables
#define MAX_USERS 10
User* user_head = NULL;
static User user_pool[MAX_USERS];
static int user_count = 0;
// reboot
void sys_reboot(void) {
    __asm__ volatile ("cli");
    __asm__ volatile (
        "movb $0xFE, %al\n"
        "outb %al, $0x64\n"
    );
    while (1) {
        __asm__ volatile ("hlt");
    }
}

void sys_poweroff(void) {
    __asm__ volatile ("cli");
    __asm__ volatile ("outw %0, %1" : : "ax"((unsigned short)0x2000), "Nd"(0x604));
    while (1) __asm__ volatile ("hlt");
}

int sys_cat(const char* filename) {
    char buffer[512];
    int size = fs_read_file(filename, buffer, FS_MAX_FILE_SIZE);
    if (size < 0) {
        puts("File not found\n");
        return -1;
    }
    buffer[size] = '\0';
    puts(buffer);
    puts("\n");
    return 0;
}
int sys_touch(const char* filename) {
    return fs_add_file(filename, "", 0);
}

int sys_append(const char* filename, const char* text) {
    char buffer[FS_MAX_FILE_SIZE];
    int size = fs_read_file(filename, buffer, FS_MAX_FILE_SIZE);
    if (size < 0) {
        puts("File not found\n");
        return -1;
    }

    int append_len = strlen(text);
    if (size + append_len >= (int)sizeof(buffer)) {
        puts("File too big to append\n");
        return -1;
    }

    memcpy(buffer + size, text, append_len);
    size += append_len;

    // Optional : add null terminator
    if (size < (int)sizeof(buffer)) {
        buffer[size] = '\0';
    }

    int res = fs_add_file(filename, buffer, size);
    if (res == 0) {
        puts("Text appended.\n");
    } else {
        puts("Failed to append.\n");
    }
    return res;
}


int sys_rm(const char* filename) {
    
    return fs_delete_file(filename);
    
}

int sys_ls(void) {
    fs_list_files();
    return 0;
}


int sys_adduser(const char* username, const char* password, int privileges) {
    User* cur = user_head;
    while (cur) {
        if (strcmp(cur->username, username) == 0) {
            printf("User already exists.\n");
            return -1;
        }
        cur = cur->next;
    }

    if (user_count >= MAX_USERS) {
        printf("User limit reached.\n");
        return -2;
    }

    User* new_user = &user_pool[user_count++];

    strncpy(new_user->username, username, sizeof(new_user->username) - 1);
    new_user->username[sizeof(new_user->username) - 1] = '\0';

    strncpy(new_user->password, password, sizeof(new_user->password) - 1);
    new_user->password[sizeof(new_user->password) - 1] = '\0';

    new_user->privileges = privileges;

    new_user->next = user_head;
    user_head = new_user;

    return 0;
}
