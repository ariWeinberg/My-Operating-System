#include "../SCREEN_DRIVER/screen_driver.h"
#include "../UTILS/utils.h"
#include "IDT.h"
#include "../MEMORY_MANAGMENT/memory_managment.h"
extern void isr0();  // your ISR stubs
extern void isr1();

idt_ptr idt_reg;
struct idt_entry idt[IDT_SIZE];




// __attribute__((naked))
// void common_isr(void) {
//     __asm__ volatile (
//         "pusha\n"
//     );

//     print_string("hello from ISR\n");

//     /* EOI to PIC (safe even if exception) */
//     __asm__ volatile (
//         "movb $0x20, %al\n"
//         "outb %al, $0x20\n"
//         "outb %al, $0xA0\n"
//     );

//     __asm__ volatile (
//         "popa\n"
//         "iret\n"
//     );
// }

// void idt_test_init(void) {
//     for (int i = 0; i < IDT_SIZE; i++) {
//         // idt_set_gate(i, (uint32_t)common_isr);
        
//         // Changed idt_set_gate to set_idt_entry
//         // Added 0x08 (Kernel Code Segment) and 0x8E (Interrupt Gate present)
//         set_idt_entry(i, (uint32_t)common_isr, 0x08, 0x8E); 
//     }

//     idt_reg.limit = sizeof(idt) - 1;
//     idt_reg.base  = (uint32_t)&idt;

//     __asm__ volatile ("lidt %0" : : "m"(idt_reg));
// }




/* This array will hold the actual machine code for 256 "stub" functions */
/* Each stub is roughly 7 bytes: push imm8 (2), jmp rel32 (5) */
uint8_t isr_stubs[256][10]; 

__attribute__((naked))
void common_isr(void) {
    __asm__ volatile (
        "pusha\n"
        // The interrupt ID is on the stack now (pushed by the stub)
        // We fetch it from the stack (ESP + 32 because of pusha)
        "mov 32(%esp), %eax\n" 
        "push %eax\n"          // Pass ID as argument to print_isr_id
        "call print_isr_id\n"
        "add $4, %esp\n"       // Clean up argument
        
        /* EOI to PIC */
        "movb $0x20, %al\n"
        "outb %al, $0x20\n"
        "outb %al, $0xA0\n"

        "popa\n"
        "add $4, %esp\n"       // Clean up the ID pushed by the stub
        "iret\n"
    );
}

// Helper function to handle the printing
void print_isr_id(uint8_t id) {
    print_string("Interrupt received: ");
    // Assuming you have a function to print numbers/hex
    char buff[5]; 
    print_string(uint32_to_str(id, buff)); 
    print_string("\n");
}

void idt_test_init(void) {
    for (int i = 0; i < IDT_SIZE; i++) {
        /* 1. Build a custom machine-code stub for this specific ID */
        // Opcode 0x6A: push imm8
        isr_stubs[i][0] = 0x6A;
        isr_stubs[i][1] = (uint8_t)i;
        
        // Opcode 0xE9: jmp rel32
        isr_stubs[i][2] = 0xE9;
        // Calculate relative offset to common_isr
        uint32_t jump_target = (uint32_t)common_isr - ((uint32_t)&isr_stubs[i][2] + 5);
        *(uint32_t*)&isr_stubs[i][3] = jump_target;

        /* 2. Point the IDT entry to our newly generated stub */
        set_idt_entry(i, (uint32_t)&isr_stubs[i], 0x08, 0x8E); 
    }

    idt_reg.limit = sizeof(idt) - 1;
    idt_reg.base  = (uint32_t)&idt;

    __asm__ volatile ("lidt %0" : : "m"(idt_reg));
}





void set_idt_entry(int n, uint32_t handler, uint16_t selector, uint8_t type_attr)
{
    idt[n].offset_low = handler & 0xFFFF;
    idt[n].selector = selector;
    idt[n].zero = 0;
    idt[n].type_attr = type_attr;
    idt[n].offset_high = (handler >> 16) & 0xFFFF;
}

extern void keyboard_isr();

void idt_init() {
    memset(idt, 0, sizeof(idt));

    idt_reg.limit = sizeof(idt) - 1;
    idt_reg.base  = (uint32_t)&idt;

    load_idt(&idt_reg);
}

void load_idt(idt_ptr *ip)
{
    __asm__ volatile ("lidt (%0)" : : "r"(ip));
}
