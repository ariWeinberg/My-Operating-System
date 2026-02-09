#ifndef UTILS
#define UTILS

#include "../STD/int.h"

char *uint16_to_str(uint16_t value, char *buffer);
uint32_t combine_u16_to_u32(uint16_t hi, uint16_t lo);
uint32_t combine_u8_to_u16(uint8_t hi, uint8_t lo);
char *uint32_to_str(uint32_t value, char *buffer);
char *uint8_to_hex_prefixed(uint8_t value, char *buffer);
char *uint8_to_hex(uint8_t value, char *buffer);
char *uint32_to_hex(uint32_t value, char *buffer);
char *uint16_to_hex(uint16_t value, char *buffer);
char *uint16_to_hex_prefixed(uint16_t value, char *buffer);
int memcmp(const void *a, const void *b, size_t n);
#endif