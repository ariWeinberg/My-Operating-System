#include "../SCREEN_DRIVER/screen_driver.h"
#include "../UTILS/utils.h"
#include "IDT.h"

/* This array will hold the actual machine code for 256 "stub" functions */
/* Each stub is roughly 7 bytes: push imm8 (2), jmp rel32 (5) */
__attribute__((section(".text")))
__attribute__((aligned(16)))
// uint8_t isr_stubs[256][16];




__attribute__((naked))
void idt_common_isr(void)
{
    asm volatile(
        "pusha\n\t"

        // interrupt number is at esp + 32
        "mov 32(%esp), %eax\n\t"
        "push %eax\n\t"
        "call idt_print_isr_id\n\t"
        "add $4, %esp\n\t"

        // EOI (safe only for IRQs, but ok for now)
        "movb $0x20, %al\n\t"
        "outb %al, $0x20\n\t"
        "outb %al, $0xA0\n\t"

        "popa\n\t"
        "add $4, %esp\n\t"   // remove interrupt_id
        "iret\n\t"
    );
}




// Helper function to handle the printing
void idt_print_isr_id(uint8_t id) {
    print_string("Interrupt received: ");
    // Assuming you have a function to print numbers/hex
    char buff[5]; 
    print_string(uint32_to_str(id, buff)); 
    print_string("\n");
}