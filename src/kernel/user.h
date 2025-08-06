
typedef enum {
    USER_ROLE_ROOT,
    USER_ROLE_USER,
} UserRole;

typedef struct User{
    char username[16];
    char password[16]; 
    int privileges; // 0= guest 1= user 2 = root
    struct User* next;
    UserRole role;
} User;

extern User* current_user;
extern User* user_head;
