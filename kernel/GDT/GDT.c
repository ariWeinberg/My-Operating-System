#include "GDT.h"


static inline gdt_entry_t gdt_make_entry(uint32_t base, uint32_t limit,
                                         uint8_t access, uint8_t flags /* upper nibble */)
{
    gdt_entry_t e;
    e.limit_low = (uint16_t)(limit & 0xFFFF);
    e.base_low  = (uint16_t)(base & 0xFFFF);
    e.base_mid  = (uint8_t)((base >> 16) & 0xFF);
    e.access    = access;
    e.gran      = (uint8_t)(((limit >> 16) & 0x0F) | (flags & 0xF0));
    e.base_high = (uint8_t)((base >> 24) & 0xFF);
    return e;
}


static gdt_entry_t gdt[3];
static gdt_ptr_t   gdtr;

extern void gdt_load(gdt_ptr_t *gdtr_ptr);

void gdt_init(void)
{
    gdt[0] = gdt_make_entry(0, 0, 0, 0);             // null
    gdt[1] = gdt_make_entry(0, 0xFFFFF, 0x9A, 0xCF); // code
    gdt[2] = gdt_make_entry(0, 0xFFFFF, 0x92, 0xCF); // data

    gdtr.limit = sizeof(gdt) - 1;
    gdtr.base  = (uint32_t)&gdt[0];

    gdt_load(&gdtr);
}

void gdt_load(gdt_ptr_t *gdtr_ptr)
{
    __asm__ volatile ("lgdt (%0)" : : "r"(gdtr_ptr));
}