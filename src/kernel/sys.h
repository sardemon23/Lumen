#ifndef SYS_H
#define SYS_H

void sys_reboot(void);
void sys_poweroff(void);
int sys_touch(const char* filename);
int sys_append(const char* filename, const char* text);
int sys_cat(const char* filename);
int sys_rm(const char* filename);
int sys_ls(void);
int sys_adduser(const char* username, const char* password, int privileges);
#endif
