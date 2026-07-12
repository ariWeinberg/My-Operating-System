#include "../STD/int.h"

#ifndef IDT_H
#define IDT_H
// IDT entry structure (32-bit interrupt gate)
struct idt_entry {
    uint16_t offset_low;  // lower 16 bits of handler address
    uint16_t selector;    // code segment selector in GDT
    uint8_t  zero;        // always 0
    uint8_t  type_attr;   // type and attributes
    // uint8_t  zero_DPL_p;   // type and attributes
    uint16_t offset_high; // higher 16 bits of handler address
} __attribute__((packed));

typedef struct idt_ptr {
    uint16_t limit;  // size of IDT - 1
    uint32_t base;   // address of IDT
} __attribute__((packed)) idt_ptr;


typedef enum gate_type
{
    TASK_GATE = 0x5,
    INT16_GATE = 0x6,
    TRAP16_GATE = 0x7,
    INT32_GATE = 0xE,
    TRAP32_GATE = 0xF,
} gate_type;

#define IDT_SIZE 256
extern uint8_t isr_stubs[256][16];
void idt_common_isr(void);


void idt_init(void);
void idt_load(idt_ptr *ip);
void idt_init_with_stubs(void);
void idt_ignore_all(void);

void idt_set_interrupt_32(int interrupt_id, void *handler);

// void idt_set_entry(int n, uint32_t handler, uint16_t selector, uint8_t type_attr);
void idt_set_entry(int n, uint32_t handler, uint16_t selector, uint8_t type_attr, uint8_t DPL);

void interrupts_disable(void);
void interrupts_enable(void);

#endif