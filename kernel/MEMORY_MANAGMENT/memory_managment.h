#ifndef MEM_MGMT
#define MEM_MGMT

#include "memory_managment_defs.h"
// typedef unsigned long size_t; // Or #include <stddef.h> if available


void *malloc(uint16_t size);
void free(void *ptr);
void *realloc(void *src, uint16_t newSize);
void *zalloc(uint16_t size);
void *memset(void *s, int c, size_t n);
void *memcpy(void *dest, const void *src, size_t n);

mem_error_t mem_get_last_error(void);
#endif
