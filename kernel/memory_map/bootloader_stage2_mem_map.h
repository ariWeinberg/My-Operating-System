#pragma once
#include "./STD/int.h"

typedef enum {
    E820_USABLE   = 1,
    E820_RESERVED = 2,
    E820_ACPI_REC = 3,
    E820_ACPI_NVS = 4,
    E820_BAD_MEM  = 5
} E820EntryType;

typedef struct __attribute__((packed)) {
    uint64_t base;
    uint64_t length;
    uint32_t type;
    uint32_t acpi3;
} E820Entry;

#define MEMORY_MAP_COUNT_ADDR 0x5000
#define MEMORY_MAP_ADDR       0x5004

#define memory_map_count (*(uint16_t*)MEMORY_MAP_COUNT_ADDR)
#define memory_map       ((E820Entry*)MEMORY_MAP_ADDR)