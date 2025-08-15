#ifndef SYS_H
#define SYS_H
#include "user.h"
#include "module/module.h"
#include "lib/string.h"


extern Module* module_head;

void sys_reboot(void);
void sys_poweroff(void);
int sys_touch(const char* filename);
int sys_append(const char* filename, const char* text);
int sys_cat(const char* filename);
int sys_rm(const char* filename);
int sys_ls(void);

void sys_permission_error(const char *message, UserRole role);

void sys_list_module(void);
void sys_insmod(const char *name) ;
char kread_char();

#endif
