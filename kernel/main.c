#include "SCREEN_DRIVER/screen_driver.h"
#include "IDT/IDT.h"
#include "IDT/ISRs/keyboard.h"
#include "PIC/PIC.h"

void kernel_setup_interrupts();
__attribute__((section(".text.boot"))) 
void _start() {
    // char a[] = "hello from kernel";
    // while(1);

    clear_screen();
    print_string("hello from kernel!\n");
    kernel_setup_interrupts();

    while(1); // Essential: do not let the CPU fall into empty memory
}

void kernel_setup_interrupts() {
    PIC_remap(0x20, 0x28);   // REQUIRED
    idt_init();
    __asm__ volatile("cli");

    idt_test_init();

    /* mask everything except keyboard if you want */
    outb(0x21, 0xFD);
    outb(0xA1, 0xFF);

    // __asm__ volatile("sti");

    init_keyboard_irq();
    IRQ_clear_mask(1); // keyboard IRQ

    __asm__ volatile("sti");
}
