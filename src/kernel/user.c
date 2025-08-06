// user.c
#include "user.h"
#include "stddef.h"
#define MAX_USERS 8
#include "stdbool.h"
#include "string.h"
#include "stdio.h"
static User users[MAX_USERS] = {
    { "root", "toor", USER_ROLE_ROOT },
    { "guest", "guest", USER_ROLE_USER },
};

User* current_user = NULL;

User* find_user(const char* username) {
    for (int i = 0; i < MAX_USERS; i++) {
        if (strcmp(users[i].username, username) == 0) {
            return &users[i];
        }
    }
    return NULL;
}

bool login(const char* username, const char* password) {
    User* u = find_user(username);
    if (!u) return false;
    if (strcmp(u->password, password) == 0) {
        current_user = u;
        return true;
    }
    return false;
}

User* users_head = NULL; // global list of all user

#define MAX_USERS 10
int user_count = 0;


