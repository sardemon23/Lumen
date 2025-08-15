#include "stdio.h"
#include "../arch/i686/io.h"
#include <stdarg.h>
#include <stdbool.h>
#include "../keyboard.h"
#include "stdint.h"
#include "stddef.h"
#include "colors.h"
#include "string.h"
const unsigned SCREEN_WIDTH = 80;
const unsigned SCREEN_HEIGHT = 25;
const uint8_t DEFAULT_COLOR = 0x7;
#define VGA_WIDTH   80
#define VGA_HEIGHT  25
#define VGA_MEMORY  0xB8000 
uint16_t* terminal_buffer = (uint16_t*)VGA_MEMORY;

uint8_t* g_ScreenBuffer = (uint8_t*)0xB8000;
int g_ScreenX = 0, g_ScreenY = 0;


void putchr(int x, int y, char c)
{
    g_ScreenBuffer[2 * (y * SCREEN_WIDTH + x)] = c;
}

void putcolor(int x, int y, uint8_t color)
{
    g_ScreenBuffer[2 * (y * SCREEN_WIDTH + x) + 1] = color;
}

char getchr(int x, int y)
{
    return g_ScreenBuffer[2 * (y * SCREEN_WIDTH + x)];
}

uint8_t getcolor(int x, int y)
{
    return g_ScreenBuffer[2 * (y * SCREEN_WIDTH + x) + 1];
}

void setcursor(int x, int y)
{
    int pos = y * SCREEN_WIDTH + x;

    i686_outb(0x3D4, 0x0F);
    i686_outb(0x3D5, (uint8_t)(pos & 0xFF));
    i686_outb(0x3D4, 0x0E);
    i686_outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}
//clear the screen
void clrscr()
{
    for (int y = 0; y < SCREEN_HEIGHT; y++)
        for (int x = 0; x < SCREEN_WIDTH; x++)
        {
            putchr(x, y, '\0');
            putcolor(x, y, DEFAULT_COLOR);
        }

    g_ScreenX = 0;
    g_ScreenY = 0;
    setcursor(g_ScreenX, g_ScreenY);
}

void clrscr_with_color(uint8_t color) {
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = vga_entry(' ', color);
        }
    }
}

void scrollback(int lines)
{
    for (int y = lines; y < SCREEN_HEIGHT; y++)
        for (int x = 0; x < SCREEN_WIDTH; x++)
        {
            putchr(x, y - lines, getchr(x, y));
            putcolor(x, y - lines, getcolor(x, y));
        }

    for (int y = SCREEN_HEIGHT - lines; y < SCREEN_HEIGHT; y++)
        for (int x = 0; x < SCREEN_WIDTH; x++)
        {
            putchr(x, y, '\0');
            putcolor(x, y, DEFAULT_COLOR);
        }

    g_ScreenY -= lines;
}

void putc(char c)
{
    switch (c)
    {
        case '\n':
            g_ScreenX = 0;
            g_ScreenY++;
            break;
    
        case '\t':
            for (int i = 0; i < 4 - (g_ScreenX % 4); i++)
                putc(' ');
            break;

        case '\r':
            g_ScreenX = 0;
            break;

        default:
            putchr(g_ScreenX, g_ScreenY, c);
            g_ScreenX++;
            break;
    }

    if (g_ScreenX >= SCREEN_WIDTH)
    {
        g_ScreenY++;
        g_ScreenX = 0;
    }
    if (g_ScreenY >= SCREEN_HEIGHT)
        scrollback(1);

    setcursor(g_ScreenX, g_ScreenY);
}

void puts(const char* str)
{
    while(*str)
    {
        putc(*str);
        str++;
    }
}

const char g_HexChars[] = "0123456789abcdef";


#define PRINTF_STATE_NORMAL         0
#define PRINTF_STATE_LENGTH         1
#define PRINTF_STATE_LENGTH_SHORT   2
#define PRINTF_STATE_LENGTH_LONG    3
#define PRINTF_STATE_SPEC           4

#define PRINTF_LENGTH_DEFAULT       0
#define PRINTF_LENGTH_SHORT_SHORT   1
#define PRINTF_LENGTH_SHORT         2
#define PRINTF_LENGTH_LONG          3
#define PRINTF_LENGTH_LONG_LONG     4

void printf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    int state = PRINTF_STATE_NORMAL;
    int length = PRINTF_LENGTH_DEFAULT;
    int radix = 10;
    bool sign = false;
    bool number = false;

    while (*fmt)
    {
        switch (state)
        {
            case PRINTF_STATE_NORMAL:
                switch (*fmt)
                {
                    case '%':   state = PRINTF_STATE_LENGTH;
                                break;
                    default:    putc(*fmt);
                                break;
                }
                break;

            case PRINTF_STATE_LENGTH:
                switch (*fmt)
                {
                    case 'h':   length = PRINTF_LENGTH_SHORT;
                                state = PRINTF_STATE_LENGTH_SHORT;
                                break;
                    case 'l':   length = PRINTF_LENGTH_LONG;
                                state = PRINTF_STATE_LENGTH_LONG;
                                break;
                    default:    goto PRINTF_STATE_SPEC_;
                }
                break;

            case PRINTF_STATE_LENGTH_SHORT:
                if (*fmt == 'h')
                {
                    length = PRINTF_LENGTH_SHORT_SHORT;
                    state = PRINTF_STATE_SPEC;
                }
                else goto PRINTF_STATE_SPEC_;
                break;

            case PRINTF_STATE_LENGTH_LONG:
                if (*fmt == 'l')
                {
                    length = PRINTF_LENGTH_LONG_LONG;
                    state = PRINTF_STATE_SPEC;
                }
                else goto PRINTF_STATE_SPEC_;
                break;

            case PRINTF_STATE_SPEC:
            PRINTF_STATE_SPEC_:
                switch (*fmt)
                {
                    case 'c':   putc((char)va_arg(args, int));
                                break;

                    case 's':   
                                puts(va_arg(args, const char*));
                                break;

                    case '%':   putc('%');
                                break;

                    case 'd':
                    case 'i':   radix = 10; sign = true; number = true;
                                break;

                    case 'u':   radix = 10; sign = false; number = true;
                                break;

                    case 'X':
                    case 'x':
                    case 'p':   radix = 16; sign = false; number = true;
                                break;

                    case 'o':   radix = 8; sign = false; number = true;
                                break;

                    // ignore invalid spec
                    default:    break;
                }

                if (number)
                {
                    if (sign)
                    {
                        switch (length)
                        {
                        case PRINTF_LENGTH_SHORT_SHORT:
                        case PRINTF_LENGTH_SHORT:
                        case PRINTF_LENGTH_DEFAULT:     printf_signed(va_arg(args, int), radix);
                                                        break;

                        case PRINTF_LENGTH_LONG:        printf_signed(va_arg(args, long), radix);
                                                        break;

                        case PRINTF_LENGTH_LONG_LONG:   printf_signed(va_arg(args, long long), radix);
                                                        break;
                        }
                    }
                    else
                    {
                        switch (length)
                        {
                        case PRINTF_LENGTH_SHORT_SHORT:
                        case PRINTF_LENGTH_SHORT:
                        case PRINTF_LENGTH_DEFAULT:     printf_unsigned(va_arg(args, unsigned int), radix);
                                                        break;
                                                        
                        case PRINTF_LENGTH_LONG:        printf_unsigned(va_arg(args, unsigned  long), radix);
                                                        break;

                        case PRINTF_LENGTH_LONG_LONG:   printf_unsigned(va_arg(args, unsigned  long long), radix);
                                                        break;
                        }
                    }
                }

                // reset state
                state = PRINTF_STATE_NORMAL;
                length = PRINTF_LENGTH_DEFAULT;
                radix = 10;
                sign = false;
                number = false;
                break;
        }

        fmt++;
    }

    va_end(args);
}

void print_buffer(const char* msg, const void* buffer, uint32_t count)
{
    const uint8_t* u8Buffer = (const uint8_t*)buffer;
    
    puts(msg);
    for (uint16_t i = 0; i < count; i++)
    {
        putc(g_HexChars[u8Buffer[i] >> 4]);
        putc(g_HexChars[u8Buffer[i] & 0xF]);
    }
    puts("\n");
}
extern char input_buffer[INPUT_BUFFER_SIZE];
extern void redraw_input_line(void);

// Buffer interne pour safe_printf
static char printf_buffer[INPUT_BUFFER_SIZE];
//static int printf_buffer_index = 0;


// Fonctions d’aide pour afficher les nombres (tu peux adapter ou utiliser les tiennes)
static char printf_buffer[INPUT_BUFFER_SIZE];
static int printf_buffer_index = 0;

void buffered_putc(char c) {
    if (printf_buffer_index < INPUT_BUFFER_SIZE - 1) {
        printf_buffer[printf_buffer_index++] = c;
        printf_buffer[printf_buffer_index] = '\0';
    }
}

void buffered_puts(const char* s) {
    while (*s) {
        buffered_putc(*s++);
    }
}

void printf_unsigned(unsigned long long num, int radix) {
    char buffer[32];
    int i = 0;
    if (num == 0) {
        buffered_putc('0');
        return;
    }
    while (num && i < (int)sizeof(buffer)) {
        buffer[i++] = g_HexChars[num % radix];
        num /= radix;
    }
    while (i--) {
        buffered_putc(buffer[i]);
    }
}

void printf_signed(long long num, int radix) {
    if (num < 0) {
        buffered_putc('-');
        num = -num;
    }
    printf_unsigned((unsigned long long)num, radix);
}


// La fonction safe_printf complète
void safe_printf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    int state = PRINTF_STATE_NORMAL;
    int length = PRINTF_LENGTH_DEFAULT;
    int radix = 10;
    bool sign = false;
    bool number = false;

    // Reset buffer
    printf_buffer_index = 0;
    printf_buffer[0] = '\0';

    while (*fmt) {
        switch (state) {
            case PRINTF_STATE_NORMAL:
                switch (*fmt) {
                    case '%': state = PRINTF_STATE_LENGTH; break;
                    default: buffered_putc(*fmt); break;
                }
                break;

            case PRINTF_STATE_LENGTH:
                switch (*fmt) {
                    case 'h': length = PRINTF_LENGTH_SHORT; state = PRINTF_STATE_LENGTH_SHORT; break;
                    case 'l': length = PRINTF_LENGTH_LONG; state = PRINTF_STATE_LENGTH_LONG; break;
                    default: goto PRINTF_STATE_SPEC_;
                }
                break;

            case PRINTF_STATE_LENGTH_SHORT:
                if (*fmt == 'h') {
                    length = PRINTF_LENGTH_SHORT_SHORT;
                    state = PRINTF_STATE_SPEC;
                } else goto PRINTF_STATE_SPEC_;
                break;

            case PRINTF_STATE_LENGTH_LONG:
                if (*fmt == 'l') {
                    length = PRINTF_LENGTH_LONG_LONG;
                    state = PRINTF_STATE_SPEC;
                } else goto PRINTF_STATE_SPEC_;
                break;

            case PRINTF_STATE_SPEC:
            PRINTF_STATE_SPEC_:
                sign = false;
                number = false;
                switch (*fmt) {
                    case 'c':
                        buffered_putc((char)va_arg(args, int));
                        break;
                    case 's':
                        buffered_puts(va_arg(args, const char*));
                        break;
                    case '%':
                        buffered_putc('%');
                        break;
                    case 'd':
                    case 'i':
                        radix = 10;
                        sign = true;
                        number = true;
                        break;
                    case 'u':
                        radix = 10;
                        sign = false;
                        number = true;
                        break;
                    case 'x':
                    case 'X':
                    case 'p':
                        radix = 16;
                        sign = false;
                        number = true;
                        break;
                    case 'o':
                        radix = 8;
                        sign = false;
                        number = true;
                        break;
                    default:
                        // Ignore unknown specifiers
                        break;
                }

                if (number) {
                    if (sign) {
                        switch (length) {
                            case PRINTF_LENGTH_SHORT_SHORT:
                            case PRINTF_LENGTH_SHORT:
                            case PRINTF_LENGTH_DEFAULT:
                                printf_signed(va_arg(args, int), radix);
                                break;
                            case PRINTF_LENGTH_LONG:
                                printf_signed(va_arg(args, long), radix);
                                break;
                            case PRINTF_LENGTH_LONG_LONG:
                                printf_signed(va_arg(args, long long), radix);
                                break;
                        }
                    } else {
                        switch (length) {
                            case PRINTF_LENGTH_SHORT_SHORT:
                            case PRINTF_LENGTH_SHORT:
                            case PRINTF_LENGTH_DEFAULT:
                                printf_unsigned(va_arg(args, unsigned int), radix);
                                break;
                            case PRINTF_LENGTH_LONG:
                                printf_unsigned(va_arg(args, unsigned long), radix);
                                break;
                            case PRINTF_LENGTH_LONG_LONG:
                                printf_unsigned(va_arg(args, unsigned long long), radix);
                                break;
                        }
                    }
                }

                state = PRINTF_STATE_NORMAL;
                length = PRINTF_LENGTH_DEFAULT;
                radix = 10;
                sign = false;
                number = false;
                break;
        }

        fmt++;
    }

    va_end(args);

    // Une fois la chaîne formée, on la copie dans input_buffer
    strncpy(input_buffer, printf_buffer, INPUT_BUFFER_SIZE - 1);
    input_buffer[INPUT_BUFFER_SIZE - 1] = '\0';

    // Et on rafraîchit la ligne de saisie pour afficher le nouveau contenu
    redraw_input_line();
}


