#ifndef GDT_H
#define GDT_H
#include "../STD/int.h"


typedef struct {
    uint16_t limit_low;     // bits 0..15 of limit
    uint16_t base_low;      // bits 0..15 of base
    uint8_t  base_mid;      // bits 16..23 of base
    uint8_t  access;        // access byte
    uint8_t  gran;          // high 4 bits = flags, low 4 bits = limit[16..19]
    uint8_t  base_high;     // bits 24..31 of base
} __attribute__((packed)) gdt_entry_t;

typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) gdt_ptr_t;


void gdt_init(void);


#endif