#include "user.h"
#include "lib/string.h"
#include "lib/stdio.h"
#include "memory/memory.h"
#include "sys.h"

#define MAX_USERS 8

static User users[MAX_USERS] = {
    { "root",  "toor",  USER_ROLE_ROOT },
    { "guest", "guest", USER_ROLE_GUEST },
};
static int user_count = 2; // car on a déjà root et guest

User* current_user = NULL;

User* find_user(const char* username) {
    for (int i = 0; i < user_count; i++) {
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

bool create_user(const char* username, const char* password, UserRole role) {
    if (!current_user || current_user->role != USER_ROLE_ROOT) {
        printf("Permission denied: Only root can create users.\n");
        return false;
    }
    if (user_count >= MAX_USERS) {
        printf("Error: Maximum number of users reached.\n");
        return false;
    }
    if (find_user(username)) {
        printf("Error: User already exists.\n");
        return false;
    }
    User* u = &users[user_count++];
    strncpy(u->username, username, sizeof(u->username)-1);
    strncpy(u->password, password, sizeof(u->password)-1);
    u->role = role;
    printf("User '%s' created successfully.\n", username);
    return true;
}

bool delete_user(const char* username) {
    if (!current_user || current_user->role != USER_ROLE_ROOT) {
        printf("Permission denied: Only root can delete users. bcs he is batman\n");
        return false;
    }
    if (strcmp(username, "root") == 0) {
        printf("Error: Cannot delete root user. He's batman\n");
        return false;
    }
    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].username, username) == 0) {
            users[i] = users[--user_count]; // remplace par le dernier
            printf("User '%s' deleted successfully.\n", username);
            return true;
        }
    }
    printf("Error: User not found.\n");
    return false;
}

bool change_password(const char* username, const char* new_password) {
    if (!current_user) {
        printf("Error: Not logged in.\n");
        return false;
    }
    // Root can change the password of anyone
    if (current_user->role != USER_ROLE_ROOT &&
        strcmp(current_user->username, username) != 0) {
        printf("Permission denied: Cannot change password for other users.\n");
        return false;
    }

    User* u = find_user(username);
    if (!u) {
        printf("Error: User not found.\n");
        return false;
    }
    strncpy(u->password, new_password, sizeof(u->password)-1);
    printf("Password changed for user '%s'.\n", username);
    return true;
}
void list_user() {
    printf("=== User List ===\n");
    for (int i = 0; i < user_count; i++) {
        
        printf("%s (role: %d)\n", users[i].username, users[i].role);
    }
}
void init_user(){
    create_user("guest", "", USER_ROLE_GUEST);
    create_user("root", "root", USER_ROLE_ROOT);
}

void whoami() {
    if (!current_user) {
        printf("No user logged in.\n");
        return;
    }

    const char *role_name = "unknown";
    switch (current_user->role) {
        case USER_ROLE_ROOT:  role_name = "root";  break;
        case USER_ROLE_USER:  role_name = "user";  break;
        case USER_ROLE_GUEST: role_name = "guest"; break;
    }

    printf("Username: %s\n", current_user->username);
    printf("Role: %s\n", role_name);
    printf("Privileges: %d\n", current_user->privileges);
}

void init_users() {
    
    for (int i = 0; i < MAX_USERS; i++) {
        users[i].username[0] = '\0';
        users[i].password[0] = '\0';
        users[i].privileges = USER_ROLE_GUEST;
        users[i].role = USER_ROLE_GUEST;
    }

    // root
    User* root = &users[user_count++];
    strncpy(root->username, "root", sizeof(root->username)-1);
    strncpy(root->password, "toor", sizeof(root->password)-1);
    root->role = USER_ROLE_ROOT;
    root->privileges = 0;

    // guest
    User* guest = &users[user_count++];
    strncpy(guest->username, "guest", sizeof(guest->username)-1);
    strncpy(guest->password, "guest", sizeof(guest->password)-1);
    guest->role = USER_ROLE_GUEST;
    guest->privileges = 2;

    // live (default user at boot)
    User* live = &users[user_count++];
    strncpy(live->username, "live", sizeof(live->username)-1);
    strncpy(live->password, "", sizeof(live->password)-1); // no password
    live->role = USER_ROLE_LIVE;
    live->privileges = 0;

    current_user = live; // start as live
}

void set_privileges(const char *username, int new_privileges) {
    if (!current_user || current_user->role != USER_ROLE_ROOT) {
        sys_permission_error("elevate", current_user->role);
        return;
    }

    User* u = find_user(username);
    if (!u) {
        printf("Unable to find %s\n", username);
        return;
    }

    if (strcmp(u->username, "root") == 0) {
        printf("Cannot change root privileges\n");
        return;
    }

    if (new_privileges < 0 || new_privileges > 2) {
        printf("Invalid privileges level: use 0->root 1->user 2->guest\n");
        return;
    }

    u->privileges = new_privileges;
    u->role = (UserRole)new_privileges;

    const char* role_name;
    switch (new_privileges) {
        case USER_ROLE_ROOT:  role_name = "root"; break;
        case USER_ROLE_USER:  role_name = "user"; break;
        case USER_ROLE_GUEST: role_name = "guest"; break;
        default: role_name = "unknown"; break;
    }

    printf("Privileges of user %s changed to %s\n", u->username, role_name);
}
