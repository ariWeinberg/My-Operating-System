#include "string.h"

// optional: simple strlen
int strlen(const char *s)
{
    int len = 0;
    while (*s++) len++;
    return len;
}

int strcmp(const char *a, const char *b)
{
    while(*a && (*a == *b)) { a++; b++; }
    return *(unsigned char*)a - *(unsigned char*)b;
}

char *strncpy(char *dest, const char *src, int n)
{
    int i;
    for(i=0;i<n && src[i]; i++) dest[i]=src[i];
    for(;i<n;i++) dest[i]=0;
    return dest;
}

char *strncat(char *dest, const char *src, int n)
{
    int i = 0;
    while(dest[i]) i++;
    int j;
    for(j=0; j<n && src[j]; j++) dest[i+j]=src[j];
    dest[i+j]=0;
    return dest;
}


/**
 * Converts a UTF-16 (ASCII-only) buffer to ASCII.
 * Non-ASCII characters are replaced with '?'.
 *
 * @param src   Pointer to UTF-16 source
 * @param len   Number of UTF-16 code units in src
 * @param dest  Pointer to ASCII destination buffer (must be at least len + 1 bytes)
 * @return      Number of ASCII bytes written (excluding null terminator)
 */
uint32_t utf16_to_ascii(uint16_t *src, uint32_t len, char *dest)
{
    for (uint32_t i = 0; i < len; i++)
    {
        uint16_t ch = src[i];
        dest[i] = (ch <= 0x7F) ? (char)ch : '?';
    }
    dest[len] = '\0';
    return len;
}
