#include "sys.h"
#include "lib/stdio.h"
#include "fs/ram_fs.h"
#include "memory/memory.h"
#include "lib/string.h"
#include "user.h"
#include "module/module.h"
#include "keyboard.h"
#include "arch/i686/io.h"
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



void sys_permission_error(const char *message, UserRole role) {
    const char *role_name = "unknown";

    switch (role) {
        case USER_ROLE_ROOT:  role_name = "root or live";  break;
        case USER_ROLE_USER:  role_name = "user";  break;
        case USER_ROLE_GUEST: role_name = "guest"; break;
        
    }

    printf("Permission denied: cannot execute '%s' as %s.\n", message, role_name);
}


void sys_list_module(void) {
    const Module *m = __start_modules;
    while (m < __stop_modules) {
        printf("Module: %s\n", m->name);
        m++;
    }
}

void sys_insmod(const char *name) {
    const Module *m = __start_modules;
    while (m < __stop_modules) {
        if (strcmp(m->name, name) == 0) {
            printf("Launching module %s...\n", m->name);
            m->start();
            return;
        }
        m++;
    }
    printf("Module '%s' not found.\n", name);
}

char kread_char() {
    uint8_t scancode = 0;

    // attendre qu'une touche soit pressée
    do {
        scancode = i686_inb(0x60);
    } while (scancode >= 0x80); // ignore les relâchements

    return scancode_to_ascii_azerty[scancode];
}
