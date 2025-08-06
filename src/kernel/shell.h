#ifndef SHELL_H
#define SHELL_H

#include <stdint.h>

//Initialize shell
void shell_init(void);

//Command process
void shell_process_command(char* command);
// To "support" the backspace (i'd rather say hide the fact that i don't know how to implement backspace)
void redraw_input_line(void);
//To print the User-> thing
void shell_default(void);
// to remove invisible character (i've passed 4 h trying to make the command work, every timer it says "Unrecognized command")
void trim_command(char* command);


#endif