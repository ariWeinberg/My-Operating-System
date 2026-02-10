#include "../SCREEN_DRIVER/screen_driver.h"
#include "../UTILS/utils.h"
#include "IDT.h"

/* This array will hold the actual machine code for 256 "stub" functions */
/* Each stub is roughly 7 bytes: push imm8 (2), jmp rel32 (5) */
uint8_t isr_stubs[256][10];

__attribute__((naked))
void common_isr(void)
{
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