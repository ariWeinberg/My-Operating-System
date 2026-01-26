#ifndef MEM_MGMT_DEFS
#define MEM_MGMT_DEFS

#include "../STD/int.h"

#define HEAP_BASE   0xF000
#define TABLE_BASE  0xE000
#define TABLE_SIZE  0x0FFF          // 4 KB metadata
#define MAX_ENTRIES (TABLE_SIZE / 4)

typedef struct mem_entry
{
    uint16_t offset;
    uint16_t size;
} mem_entry;

typedef enum {
    MEM_OK = 0,
    MEM_ERR_NOT_INITIALIZED,
    MEM_ERR_ZERO_SIZE,
    MEM_ERR_NO_METADATA,
    MEM_ERR_HEAP_EXHAUSTED,
    MEM_ERR_INVALID_PTR,
    MEM_ERR_REALLOC_NOT_FOUND
} mem_error_t;

#endif
