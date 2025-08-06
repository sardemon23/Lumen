#include "shell.h"
#include "panic.h"
#include "ram_fs.h"
#include "stdio.h"
#include "string.h"
#include "sys.h"
#include "timer.h"
#include <stdint.h>
#include <stdio.h>
#include "stdio.h"
#include "stdio.h"
#include "util/sys_log.h"
#include "user.h"
#define STREQ(a, b) (strcmp((a), (b)) == 0)

void shell_default(void) {
    if (current_user) {
        printf("\n%s> ", current_user->username);
    } else {
        printf("\nguest> ");
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
    trim_command(command); // Nettoie espaces au début et à la fin

    // Ne pas modifier 'command', on cherche juste le premier espace pour séparer cmd et args
    char* args = NULL;
    char* space = strchr(command, ' ');
    if (space) {
        *space = '\0';  // On coupe temporairement pour isoler le cmd principal
        args = space + 1;
        trim_command(args);  // Nettoyer args aussi
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
        log_write(LOG_INFO, "MANUAL KERNEL PANIC TRIGGERED");
        panic("MANUAL KERNEL PANIC TRIGGERED");
    }
    else if (STREQ(command, "poweroff")) {
        sys_poweroff();
    }
    else if (STREQ(command, "reboot")) {
        sys_reboot();
    }
    else if (STREQ(command, "login")) {
        // login attend 2 arguments : username et password dans args
        if (!args) {
            printf("Usage: login [user] [pass]\n");
            return;
        }
        // extraire user et pass dans args
        char* pass = strchr(args, ' ');
        if (!pass) {
            printf("Usage: login [user] [pass]\n");
            return;
        }
        *pass = '\0';
        pass++;

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
    else if (STREQ(command, "touch")) {
        if (args) {
            sys_touch(args);
            log_write(LOG_CREATED,args);
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
    else if (STREQ(command, "ls")) {
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
        printf("This is version 1.0\n");
    }
    else {
        printf("Unknown command: %s\n", command);
    }

    // Remettre espace si on veut réutiliser command plus tard (optionnel)
    if (space) *space = ' ';
}
