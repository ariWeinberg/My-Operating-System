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
    test_threading();
    kernel_setup_interrupts();

    while(1); // Essential: do not let the CPU fall into empty memory
}

void kernel_setup_interrupts() {
    PIC_remap(0x20, 0x28);   // REQUIRED
    idt_init();
    __asm__ volatile("cli");

    idt_init_with_stubs();

    /* mask everything except keyboard if you want */
    outb(0x21, 0xFC);
    outb(0xA1, 0xFF);

    // __asm__ volatile("sti");

    init_keyboard_irq();
    init_timer_IRQ();
    IRQ_clear_mask(1); // keyboard IRQ

    __asm__ volatile("sti");
}














// test threading...
#include "MEMORY_MANAGMENT/memory_managment.h"
#include "UTILS/utils.h"


typedef struct {
    uint32_t esp;
    uint32_t stack_base;
    int active;
} __attribute__((packed)) Task;
// If 3 fields, the multiplier in ASM becomes 12 instead of 8.


#define MAX_TASKS 4
Task task_list[MAX_TASKS];
int task_list_size = 0;
int current_task_index = 0;



void create_task(int slot, void (*func)()) {
    // 1. Allocate a stack (e.g., 4KB)
    uint32_t* stack = (uint32_t*)(malloc(4096) + 4096);
    
    // 2. Build the IRET frame (Hardware expected state)
    *(--stack) = 0x202;           // EFLAGS: Interrupts enabled
    *(--stack) = 0x08;            // CS: Kernel Code Segment
    *(--stack) = (uint32_t)func;  // EIP: Start of function

    // 3. Build the PUSHAL frame (Software saved state)
    *(--stack) = 0; // EAX
    *(--stack) = 0; // ECX
    *(--stack) = 0; // EDX
    *(--stack) = 0; // EBX
    *(--stack) = 0; // ESP (ignored by popal)
    *(--stack) = 0; // EBP
    *(--stack) = 0; // ESI
    *(--stack) = 0; // EDI

    // 4. Save the ESP into our task list
    task_list[slot].esp = (uint32_t)stack;
    task_list[slot].active = 1;
}




// __attribute__((naked))
void context_switch() {
    // 2. Swap stack pointers (AT&T: mov src, dest)
    asm volatile(
        "movl %%esp, %0\n\t"    // Save current %esp to task_list[index].esp
        "movl %1, %%esp"        // Load new %esp from next_task_esp
        : "=m"(task_list[current_task_index].esp) 
        : "m"(task_list[(current_task_index + 1) % task_list_size].esp)
    );

    // 3. Update task management logic in C (simplified)
    current_task_index = (current_task_index + 1) % task_list_size;
}

void task1(void) {
    while(1) {
        print_string("A ");
        for(volatile int i=0; i<1000000; i++); // Small delay
    }
}
void task2(void) {
    while(1) {
        print_string("B ");
        for(volatile int i=0; i<1000000; i++); // Small delay
    }
}

void test_threading() {
    // Task 0 is the "placeholder" for the main kernel execution
    task_list[0].active = 1; 
    
    // Create actual tasks starting at index 1
    create_task(1, &task1);
    create_task(2, &task2);
    
    task_list_size = 3;
    current_task_index = 0; // The ISR will now save the kernel here first
}


uint32_t timer_ticks = 0;
uint16_t ticks_per_task = 1;
// simple C handler for keyboard scan codes


// assembly ISR wrapper
__attribute__((naked)) void timer_isr(void)
{
    asm volatile (
        "pushal\n\t"

        "incl timer_ticks\n\t"
        "movl timer_ticks, %%eax\n\t"
        "xorl %%edx, %%edx\n\t"
        "divl ticks_per_task\n\t"
        "cmpl $0, %%edx\n\t"
        "jne skip_switch\n\t"

        // --- CONTEXT SWITCH START ---
        "movl current_task_index, %%eax\n\t"
        "imull $12, %%eax, %%ebx\n\t"            // EBX = current_task_index * 12
        "movl %%esp, task_list(%%ebx)\n\t"       // Save ESP to task_list[ebx].esp

        "incl current_task_index\n\t"            // Increment the index
        "movl current_task_index, %%eax\n\t"
        "cmpl task_list_size, %%eax\n\t"         // Check for wrap-around
        "jl load_next\n\t"
        "movl $0, current_task_index\n\t"        // Reset index to 0

    "load_next:\n\t"
        "movl current_task_index, %%eax\n\t"
        "imull $12, %%eax, %%ebx\n\t"            // EBX = new current_task_index * 12
        "movl task_list(%%ebx), %%esp\n\t"       // Load new ESP
        // --- CONTEXT SWITCH END ---

    "skip_switch:\n\t"
        "movb $0x20, %%al\n\t"
        "outb %%al, $0x20\n\t"
        "popal\n\t"
        "iret\n\t"
        : : : "eax", "ebx", "edx", "memory"      // Added memory to clobber list
    );
}


void init_timer_IRQ(void)
{
    // keyboard is IRQ0 -> IDT entry 0x20 after PIC remap
    set_idt_entry(0x20, (uint32_t)timer_isr, 0x08, 0x8E);
}