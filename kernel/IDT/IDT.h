#include "../STD/int.h"

// IDT entry structure (32-bit interrupt gate)
struct idt_entry {
    uint16_t offset_low;  // lower 16 bits of handler address
    uint16_t selector;    // code segment selector in GDT
    uint8_t  zero;        // always 0
    uint8_t  type_attr;   // type and attributes
    uint16_t offset_high; // higher 16 bits of handler address
} __attribute__((packed));

typedef struct idt_ptr {
    uint16_t limit;  // size of IDT - 1
    uint32_t base;   // address of IDT
} __attribute__((packed)) idt_ptr;

#define IDT_SIZE 256
extern uint8_t isr_stubs[256][10];
void common_isr(void);


void idt_init();
void load_idt(idt_ptr *ip);
void idt_init_with_stubs(void);

void set_idt_entry(int n, uint32_t handler, uint16_t selector, uint8_t type_attr);