#include "../STD/int.h"
#include "../STD/defs.h"
#include "utils.h"


char *uint16_to_str(uint16_t value, char *buffer)
{
    char *p = buffer;

    if (value == 0) {
        *p++ = '0';
        *p = 0;
        return buffer;
    }

    while (value > 0) {
        *p++ = '0' + (value % 10);
        value /= 10;
    }

    *p = 0;

    // reverse in-place
    char *start = buffer;
    char *end = p - 1;
    while (start < end) {
        char tmp = *start;
        *start++ = *end;
        *end-- = tmp;
    }

    return buffer;
}

uint32_t combine_u16_to_u32(uint16_t hi, uint16_t lo)
{
    return ((uint32_t)hi << 16) | (uint32_t)lo;
}
uint32_t combine_u8_to_u16(uint8_t hi, uint8_t lo)
{
    return ((uint16_t)hi << 8) | (uint16_t)lo;
}
char *uint32_to_str(uint32_t value, char *buffer)
{
    char *p = buffer;

    if (value == 0) {
        *p++ = '0';
        *p = 0;
        return buffer;
    }

    while (value > 0) {
        *p++ = '0' + (value % 10);
        value /= 10;
    }

    *p = 0;

    // reverse
    char *start = buffer;
    char *end = p - 1;
    while (start < end) {
        char t = *start;
        *start++ = *end;
        *end-- = t;
    }

    return buffer;
}

char *uint8_to_hex_prefixed(uint8_t value, char *buffer)
{
    static const char hex[] = "0123456789ABCDEF";

    buffer[0] = '0';
    buffer[1] = 'x';
    buffer[2] = hex[(value >> 4) & 0x0F];
    buffer[3] = hex[value & 0x0F];
    buffer[4] = 0;

    return buffer;
}
char *uint8_to_hex(uint8_t value, char *buffer)
{
    static const char hex[] = "0123456789ABCDEF";

    buffer[0] = hex[(value >> 4) & 0x0F];
    buffer[1] = hex[value & 0x0F];
    buffer[2] = 0;

    return buffer;
}
char *uint32_to_hex(uint32_t value, char *buffer)
{
    static const char hex[] = "0123456789ABCDEF";

    for (int i = 0; i < 8; i++) {
        // extract the nibble from highest to lowest
        buffer[i] = hex[(value >> ((7 - i) * 4)) & 0xF];
    }

    buffer[8] = 0;
    return buffer;
}


char *uint16_to_hex(uint16_t value, char *buffer)
{
    static const char hex[] = "0123456789ABCDEF";

    buffer[0] = hex[(value >> 12) & 0xF];  // highest nibble
    buffer[1] = hex[(value >> 8) & 0xF];
    buffer[2] = hex[(value >> 4) & 0xF];
    buffer[3] = hex[value & 0xF];          // lowest nibble
    buffer[4] = 0;                         // null terminator

    return buffer;
}

char *uint16_to_hex_prefixed(uint16_t value, char *buffer)
{
    static const char hex[] = "0123456789ABCDEF";

    buffer[0] = '0';
    buffer[1] = 'x';
    buffer[2] = hex[(value >> 12) & 0xF];
    buffer[3] = hex[(value >> 8) & 0xF];
    buffer[4] = hex[(value >> 4) & 0xF];
    buffer[5] = hex[value & 0xF];
    buffer[6] = 0;

    return buffer;
}


int memcmp(const void *a, const void *b, size_t n)
{
    const uint8_t *p1 = (const uint8_t *)a;
    const uint8_t *p2 = (const uint8_t *)b;

    for (size_t i = 0; i < n; i++)
    {
        if (p1[i] != p2[i])
            return (int)p1[i] - (int)p2[i];
    }

    return 0;
}