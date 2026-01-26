#ifndef MEM_MGMT
#define MEM_MGMT

#include "memory_managment_defs.h"

static void mem_init(void);
void *malloc(uint16_t size);
void free(void *ptr);
void *realloc(void *src, uint16_t newSize);
void *zalloc(uint16_t size);

mem_error_t mem_get_last_error(void);
#endif
