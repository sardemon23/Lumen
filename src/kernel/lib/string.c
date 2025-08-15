#include "string.h"
#include <stdint.h>
#include <stddef.h>

const char* strchr(const char* str, char chr)
{
    if (str == NULL)
        return NULL;

    while (*str)
    {
        if (*str == chr)
            return str;

        ++str;
    }

    return NULL;
}

char* strcpy(char* dst, const char* src)
{
    char* origDst = dst;

    if (dst == NULL)
        return NULL;

    if (src == NULL)
    {
        *dst = '\0';
        return dst;
    }

    while (*src)
    {
        *dst = *src;
        ++src;
        ++dst;
    }
    
    *dst = '\0';
    return origDst;
}

unsigned strlen(const char* str)
{
    unsigned len = 0;
    while (*str)
    {
        ++len;
        ++str;
    }

    return len;
}
int strcmp(const char* s1, const char* s2)
{
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

int strncmp(const char* s1, const char* s2, unsigned int n)
{
    while (n && *s1 && (*s1 == *s2)) {
        s1++;
        s2++;
        n--;
    }
    return n ? *(const unsigned char*)s1 - *(const unsigned char*)s2 : 0;
}
unsigned strnlen(const char* str, unsigned maxlen) {
    unsigned i;
    for (i = 0; i < maxlen; i++) {
        if (str[i] == '\0')
            return i;
    }
    return maxlen;
}
char* strncpy(char* dest, const char* src, size_t n) {
    size_t i;
    for (i = 0; i < n && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }
    for (; i < n; i++) {
        dest[i] = '\0';
    }
    return dest;
}
int atoi(const char* str) {
    int result = 0;
    int i = 0;
    int sign = 1;

    
    if (str[0] == '-') {
        sign = -1;
        i++;
    }

    for (; str[i] != '\0'; i++) {
        if (str[i] < '0' || str[i] > '9') {
            break;  // stop if its not a number
        }
        result = result * 10 + (str[i] - '0');
    }

    return sign * result;
}

void itoa(int value, char* str, int base) {
    char* ptr = str;
    char* ptr1 = str;
    char tmp_char;
    int tmp_value;

    if (value == 0) {
        *ptr++ = '0';
        *ptr = '\0';
        return;
    }

    int is_negative = 0;
    if (value < 0 && base == 10) {
        is_negative = 1;
        value = -value;
    }

    while (value != 0) {
        tmp_value = value % base;
        if (tmp_value < 10) *ptr++ = '0' + tmp_value;
        else *ptr++ = 'A' + (tmp_value - 10);
        value /= base;
    }

    if (is_negative) *ptr++ = '-';
    *ptr-- = '\0';

    // reverse string
    while (ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr-- = *ptr1;
        *ptr1++ = tmp_char;
    }
}
