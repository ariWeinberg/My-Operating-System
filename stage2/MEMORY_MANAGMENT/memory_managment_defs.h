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

#endif
