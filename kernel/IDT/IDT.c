#include "../SCREEN_DRIVER/screen_driver.h"
#include "../UTILS/utils.h"
#include "../STD/defs.h"
#include "IDT.h"
#include "../MEMORY_MANAGMENT/memory_managment.h"
extern void isr0();  // your ISR stubs
extern void isr1();

idt_ptr idt_reg;
struct idt_entry idt[IDT_SIZE];
uint8_t isr_stubs[256][16];



void idt_init(void) {
    memset(idt, 0, sizeof(idt));
    memset(isr_stubs, 0, sizeof(isr_stubs));

    idt_reg.limit = sizeof(idt) - 1;
    idt_reg.base  = (uint32_t)&idt;

    idt_load(&idt_reg);
}


uint8_t *idt_create_stub(void *isr, uint8_t interrupt_id) 
{
    /* 1. Build a custom machine-code stub for this specific ID */
    // Opcode 0x6A: push imm8
    uint8_t *isr_stub = &isr_stubs[interrupt_id];
    isr_stub[0] = 0x6A;
    isr_stub[1] = (uint8_t)interrupt_id;
    
    // Opcode 0xE9: jmp rel32
    isr_stub[2] = 0xE9;
    // Calculate relative offset to isr
    uint32_t jump_target = (uint32_t)isr - ((uint32_t)&isr_stub[2] + 5);
    *(uint32_t*)&isr_stub[3] = jump_target;

    return isr_stub;
}

void idt_ignore_interrupt(uint8_t interrupt_id)
{
    uint8_t *stub = idt_create_stub(&idt_common_isr, interrupt_id);
    if (!stub) return;
    idt_set_entry(interrupt_id, (uint32_t)stub, 0x08, 0x8E, 0);
    return;
}


void idt_ignore_all(void)
{
    for (int i = 0; i < IDT_SIZE; i++)
    {
        idt_ignore_interrupt(i);
    }
}


void idt_set_interrupt_32(int interrupt_id, void *handler)
{
    idt_set_entry(interrupt_id, (uint32_t)idt_create_stub(handler, interrupt_id) , 0x08, 0x8E, 0);
}










// void idt_init_with_stubs(void) {
//     for (int i = 0; i < IDT_SIZE; i++) {
//         /* 1. Build a custom machine-code stub for this specific ID */
//         // Opcode 0x6A: push imm8
//         isr_stubs[i][0] = 0x6A;
//         isr_stubs[i][1] = (uint8_t)i;
        
//         // Opcode 0xE9: jmp rel32
//         isr_stubs[i][2] = 0xE9;
//         // Calculate relative offset to idt_common_isr
//         uint32_t jump_target = (uint32_t)idt_common_isr - ((uint32_t)&isr_stubs[i][2] + 5);
//         *(uint32_t*)&isr_stubs[i][3] = jump_target;

//         /* 2. Point the IDT entry to our newly generated stub */
//         idt_set_entry(i, (uint32_t)&isr_stubs[i], 0x08, 0x8E); 
//     }

//     idt_reg.limit = sizeof(idt) - 1;
//     idt_reg.base  = (uint32_t)&idt;

//     __asm__ volatile ("lidt %0" : : "m"(idt_reg));
// }


void idt_set_entry(int n, uint32_t handler, uint16_t selector, uint8_t type_attr, uint8_t DPL)
{
    idt[n].offset_low = handler & 0xFFFF;
    idt[n].selector = selector;
    idt[n].zero = 0;
    // idt[n].type_attr = type_attr | ((0 | (DPL >> 1) | (1 >> 4)) >> 4);
    idt[n].type_attr = 0x80 | ((DPL & 3) << 5) | (type_attr & 0x0F);
    // idt[n].zero_DPL_p = 0 | (DPL >> 1) | (1 >> 8);
    idt[n].offset_high = (handler >> 16) & 0xFFFF;
}

// extern void keyboard_isr();

// void idt_init() {
//     memset(idt, 0, sizeof(idt));

//     idt_reg.limit = sizeof(idt) - 1;
//     idt_reg.base  = (uint32_t)&idt;

//     idt_load(&idt_reg);
// }

void idt_load(idt_ptr *ip)
{
    __asm__ volatile ("lidt (%0)" : : "r"(ip));
}


void interrupts_disable(void)
{
    __asm__ volatile("cli");
}

void interrupts_enable(void)
{
    __asm__ volatile("sti");
}
