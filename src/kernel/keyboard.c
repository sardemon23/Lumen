#include "keyboard.h"
#include "arch/i686/pic.h"
#include "shell.h"
#include "stdio.h"
#include "user.h"
#include "arch/i686/io.h"
#include "shell.h"
#include "string.h"
#include <stdio.h>
//define some variable
#define INPUT_BUFFER_SIZE 256

char input_buffer[INPUT_BUFFER_SIZE];
int input_index = 0;

extern User* current_user;

void redraw_input_line() {
    printf("\r");
    if (current_user && current_user->username) {
        printf("%s> %s", current_user->username, input_buffer);
    } else {
        printf("guest> %s", input_buffer);
    }
    printf(" ");
    printf("\r");
}

// scancode table of AZERTY , maybe i'll add QWERTY

unsigned char scancode_to_ascii_azerty[256] = {
    [0x01] = 27,    // ESC
    [0x02] = '1',
    [0x03] = '2',
    [0x04] = '3',
    [0x05] = '4',
    [0x06] = '5',
    [0x07] = '6',
    [0x08] = '7',
    [0x09] = '8',
    [0x0A] = '9',
    [0x0B] = '0',
    [0x0C] = '\'',
    [0x0D] = '^',
    [0x0E] = '\b',  // Backspace

    [0x0F] = '\t',  // Tab

    [0x10] = 'a',  // Q key on azerty is scancode 0x10 but letter 'a' on azerty layout
    [0x11] = 'z',  // W key = z
    [0x12] = 'e',
    [0x13] = 'r',
    [0x14] = 't',
    [0x15] = 'y',
    [0x16] = 'u',
    [0x17] = 'i',
    [0x18] = 'o',
    [0x19] = 'p',
    [0x1A] = '^',
    [0x1B] = '$',

    [0x1C] = '\n',  // Enter

    [0x1E] = 'q',   // A key = q
    [0x1F] = 's',
    [0x20] = 'd',
    [0x21] = 'f',
    [0x22] = 'g',
    [0x23] = 'h',
    [0x24] = 'j',
    [0x25] = 'k',
    [0x26] = 'l',
    [0x27] = 'm',
    [0x28] = '0xF9',
    [0x29] = '`',

    [0x2B] = '*',

    [0x2C] = 'w',   // Z key = w
    [0x2D] = 'x',
    [0x2E] = 'c',
    [0x2F] = 'v',
    [0x30] = 'b',
    [0x31] = 'n',
    [0x32] = ',', 
    [0x33] = ';',
    [0x34] = ':',
    [0x35] = '!',
    
    [0x39] = ' ',   // Space bar

    /*
    it was hard.....
    */
};

void keyboard_handler(Registers* regs) {
    uint8_t scancode = i686_inb(0x60);
    if (scancode < 0x80) // Key pressed 
    {
        if (scancode == 0x0E) { // backspace
            if (input_index > 0) {
                //redraw the input line with the content of the buffer
                input_index--;
                input_buffer[input_index] = '\0';
                redraw_input_line();
            }
        } else {
            char c = scancode_to_ascii_azerty[scancode]; 
            if (c && input_index < INPUT_BUFFER_SIZE - 1) {
                input_buffer[input_index++] = c;
                input_buffer[input_index] = '\0';
                redraw_input_line();
            }
}

    }
    if (scancode == 0x1C) {  // Enter
        if (input_index == 0)
            return;

        input_buffer[input_index] = '\0';
        printf("\n");

        shell_process_command(input_buffer); 

        input_index = 0;
        input_buffer[0] = '\0';

        shell_default(); 
        return;
    }
}


void keyboard_initialize()
{
    //listen to the keyboard
    i686_IRQ_RegisterHandler(1 , keyboard_handler);
    i686_PIC_Unmask(1);
}