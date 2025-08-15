#include "../kernel.h"


void calc(void) {
    char buffer[32];
    printf("Tape qlq chose: ");
    
    get_string_scancode(buffer, sizeof(buffer));
    printf("You writen %s\n", buffer);
}


MODULE_REGISTER("calc", calc);