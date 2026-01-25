#ifndef STRING
#define STRING

#include "../STD/int.h"

int strlen(const char *s);
int strcmp(const char *a, const char *b);
char *strncpy(char *dest, const char *src, int n);
char *strncat(char *dest, const char *src, int n);
// Convert UTF-16 (2 bytes per character) to ASCII
// src: pointer to UTF-16 array
// len: number of UTF-16 characters in src
// dest: pointer to buffer where ASCII string will be written (must have at least len+1 bytes)
// Returns: number of bytes written (excluding null terminator)
uint32_t utf16_to_ascii(uint16_t *src, uint32_t len, char *dest);
uint16_t strsplit(const char *str, const char delimiter, char ***dest);
#endif