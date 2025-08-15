#include "shell.h"

#include "panic.h"
#include "fs/ram_fs.h"
#include "lib/stdio.h"
#include "lib/string.h"
#include "sys.h"
#include "util/timer.h"
#include <stdint.h>
#include "util/sys_log.h"
#include "user.h"
#include "module/module.h"

#define STREQ(a, b) (strcmp((a), (b)) == 0)



void shell_default(void) {
    if (current_user) {
        printf("\n%s> ", current_user->username);
    } else if (!current_user) {
        printf("Unknown> ");
    }
}

void shell_init(void)
{
    timer_initialize(100);

    timer_wait(1 * 50);

    shell_default();
    
}

void trim_command(char* command) {
    int len = strlen(command);
    while (len > 0 && (command[len - 1] == ' ' || command[len - 1] == '\r' || command[len - 1] == '\n')) {
        command[len - 1] = '\0';
        len--;
    }
}

void shell_process_command(char* command) {
    trim_command(command); // Clean spaces at the start and the end

    
    char* args = NULL;
    char* space = strchr(command, ' ');
    if (space) {
        *space = '\0';  
        args = space + 1;
        trim_command(args);  // Clean args too
    }

    printf("Processing command: '%s'\n", command);

    if (STREQ(command, "help")) {
        printf("Available commands:\n");
        printf("  help        - Show this help message\n");
        printf("  clear       - Clear the screen\n");
        printf("  echo        - Print text\n");
        printf("  uptime      - Show system uptime in seconds\n");
        printf("  log         - Dump system log\n");
        printf("  log add     - Add a message to the system log\n");
        printf("  reboot      - Reboot the system\n");
        printf("  poweroff    - Stop the computer\n");
        printf("  cat         - Display the content of a file\n");
        printf("  touch       - Create an empty file\n");
        printf("  append      - Append text to a file\n");
        printf("  rm          - Remove a file\n");
        printf("  ls          - List files\n");
        printf("  version     - Show OS version\n");
        printf("  login       - Login with username and password\n");
        printf("  panic       - Make the Kernel panic intentionally\n");
        printf("  useradd     - Create a user with a defined username and password\n");
        printf("  userdel     - Delete a user\n");
        printf("  userlist    - List all user\n");
        printf("  whoami      - Obviously no need to describe\n");
        printf("  passwd      - Change the password of a defined user \n");
        printf("  elevate     - Elevate the permission of an user\n");
    }
    else if (STREQ(command, "log")) {
        // Si args commence par "add "
        if (args && strncmp(args, "add ", 4) == 0) {
            const char* msg = args + 4;
            log_write(LOG_INFO, msg);
            printf("Message added to log.\n");
        } else {
            // Pas de sous-commande, on affiche le log
            log_dump();
        }
    }
    else if (STREQ(command, "panic")) {
        if ((current_user->role = USER_ROLE_ROOT || current_user->role == USER_ROLE_LIVE)){
            log_write(LOG_INFO, "MANUAL KERNEL PANIC TRIGGERED");
            panic("MANUAL KERNEL PANIC TRIGGERED");
        }else {
            sys_permission_error(command, USER_ROLE_ROOT);
        }
    }
    else if (STREQ(command, "whoami")) {
        whoami();
    }else if (STREQ(command, "poweroff")) {
        sys_poweroff();
    }
    else if (STREQ(command, "reboot")) {
        sys_reboot();
    }
    else if (STREQ(command, "login")) {
        char* pass = NULL;

        if (!args) {
            printf("Usage: login [user] [pass]\n");
            return;
        }

        // Cherche un espace après le nom d'utilisateur
        char* space = strchr(args, ' ');
        if (space) {
            *space = '\0';     // coupe après le nom
            pass = space + 1;  // mot de passe juste après
        } else {
            pass = ""; // pas de mot de passe fourni
        }

        if (login(args, pass)) {
            printf("Welcome, %s!\n", current_user->username);
        } else {
            printf("Login failed.\n");
        }
    }

    else if (STREQ(command, "echo")) {
        if (args) {
            printf("%s\n", args);
        } else {
            printf("\n");
        }
    }
    else if (STREQ(command, "insmod")) {
        if (args){
            sys_insmod(args);
        } else{
            printf("Usage: insmod <name>\n");
        }
    }else if (STREQ(command, "lsmod")) {
        sys_list_module();
    }else if (STREQ(command, "touch")) {
        if (args) {
            sys_touch(args);
            log_write(LOG_CREATED,args);
            printf("%s Created", args);
        } else {
            printf("Usage: touch <filename>\n");
        }
    }
    else if (STREQ(command, "append")) {
        if (!args) {
            printf("Usage: append <filename> <text>\n");
            return;
        }
        char* space2 = strchr(args, ' ');
        if (!space2) {
            printf("Usage: append <filename> <text>\n");
            return;
        }
        *space2 = '\0';
        const char* filename = args;
        const char* text = space2 + 1;
        sys_append(filename, text);
        log_write(LOG_INFO, "File edited");
    }

    else if (STREQ(command, "cat")) {
        if (args) {
            sys_cat(args);
        } else {
            printf("Usage: cat <filename>\n");
        }
    }
    else if (STREQ(command, "rm")) {
        if (args) {
            sys_rm(args);
            log_write(LOG_DELETED, args);
        } else {
            printf("Usage: rm <filename>\n");
        }
    }
    else if (STREQ(command, "useradd")) {
        if (args) {
            
            char *username = args;
            char *password = NULL;

            // search the first space 
            char *space = strchr(args, ' ');
            if (space) {
                *space = '\0';          // cut the chain at the space
                password = space + 1;   // password start right after the space
            }

            if (password && password[0] != '\0') {
                create_user(username, password, USER_ROLE_GUEST);
            } else {
                printf("Invalid command: need username and password\n");
            }
        } else {
            printf("Invalid command: missing username and password\n");
        }
    }
    else if (STREQ(command, "userlist")) {
        list_user();
    }else if (STREQ(command, "userdel")) {
        if (args && args[0] != '\0') {
            delete_user(args); 
        } else {
            printf("Invalid command: missing username\n");
        }
    }
    else if (STREQ(command, "passwd")) {
        if (!args || args[0] == '\0') {
            printf("Invalid command: missing arguments\n");
            return;
        }

        char *username = NULL;
        char *newpass = NULL;

        if (current_user->privileges == 0 ) {
            // Root: passwd <username> <newpass>
            username = args;

            char *space = strchr(args, ' ');
            if (space) {
                *space = '\0';
                newpass = space + 1;
            }
        } else {
            // User normal: passwd <newpass>
            printf("Cannot change password if not root");
        }

        if (!newpass || newpass[0] == '\0') {
            printf(" Invalid command: missing new password\n");
            return;
        }

        change_password(username, newpass);
    }else if (STREQ(command, "elevate")) {
        if (!args) {
            printf("Usage: setpriv [username] [level]\n");
            return;
        }

        char* space = strchr(args, ' ');
        if (!space) {
            printf("Usage: setpriv [username] [level]\n");
            return;
        }

        *space = '\0';
        char* username = args;
        int level = atoi(space + 1); // convert to int

        set_privileges(username, level);
    }else if (STREQ(command, "ls")) {
        fs_list_files();
    }
    else if (STREQ(command, "uptime")) {
        int sec = uptime_seconds();
        printf("Uptime: %d Second%s\n", sec, (sec > 1 ? "s" : ""));
    }
    else if (STREQ(command, "clear")) {
        clrscr();
    }
    else if (STREQ(command, "version")) {
        char buffer[64];
        dtoa(VERSION, buffer, 1);
        printf("Current Version: %s\n", buffer);
    }
    else {
        printf("Unknown command: %s\n", command);
    }

    
    if (space) *space = ' ';
}
