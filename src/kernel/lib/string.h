#pragma once
#include <stddef.h>

const char* strchr(const char* str, char chr);
char* strcpy(char* dst, const char* src);
unsigned strlen(const char* str);
int strcmp(const char* s1, const char* s2);
int strncmp(const char* s1, const char* s2, unsigned int n);
char* strncpy(char* dest, const char* src, size_t n);
unsigned strnlen(const char* str, unsigned maxlen);
int atoi(const char* str);
void itoa(int value, char* str, int base);