#ifndef USER_H
#define USER_H

#include <stdbool.h>

typedef enum {
    USER_ROLE_ROOT,
    USER_ROLE_USER,
    USER_ROLE_GUEST,
    USER_ROLE_LIVE = 0,
} UserRole;

typedef struct User {
    char username[16];
    char password[16];
    int privileges; // 0-> root 1-> user 2-> guest live-> root
    int next;
    UserRole role;
} User;

extern User* current_user;

bool login(const char* username, const char* password);
User* find_user(const char* username);


bool create_user(const char* username, const char* password, UserRole role);
bool delete_user(const char* username);
bool change_password(const char* username, const char* new_password);
void list_user() ;
void whoami();
void init_users();
void set_privileges(const char* username, int new_privileges);

#endif
