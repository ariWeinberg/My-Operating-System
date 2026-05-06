#include "SCREEN_DRIVER/screen_driver.h"
#include "IDT/IDT.h"
#include "IDT/ISRs/exceptions/exceptions.h"
#include "IDT/ISRs/keyboard/keyboard.h"
#include "PIC/PIC.h"
#include "PIT/PIT.h"
#include "UTILS/utils.h"
#include "STD/bool.h"
#include "GDT/GDT.h"



void timer_handler(void);
void timer_isr(void);

uint32_t ticks = 0;
void sleep(uint16_t ms);

bool do_sched = false;


void task_a() {while(1){print_string("A"); sleep(500);}}
void task_b() {while(1){print_string("B"); sleep(2000);}}

typedef struct {
    uint32_t esp;
} Task;

Task task_list[3]; 
int current_index = 0;
int task_count = 1; 

// Pointer to the task currently owning the CPU
Task* volatile running_task = &task_list[0];
void create_task(void (*func)());



static void kernel_setup_interrupts(void);
__attribute__((section(".text.boot"))) 
void _start(void)
{
    gdt_init();
    clear_screen();
    print_string("hello from kernel!\n");
    kernel_setup_interrupts();
    while(1)
    {
        if(current_char_out < current_char_in)
        {
            put_char(char_buffer[current_char_out++], 0x0F);
        }
        
        if (do_sched)
        {
            // do_sched = false;
            // schedule_next_task();
        }
    }
    while(1); // Essential: do not let the CPU fall into empty memory
}

static void kernel_setup_interrupts(void) {
    interrupts_disable();
    pic_remap(0x20, 0x28);
    idt_init();
    idt_ignore_all();
    setup_exceptions();
    irq_mask_all();
    init_keyboard_irq();
    idt_set_interrupt_32(0x20, &timer_isr);
    irq_clear_mask(0); // timer IRQ
    irq_clear_mask(1); // keyboard IRQ

    create_task(&task_a);
    create_task(&task_b);

    pit_init(1000);
    interrupts_enable();
}


void timer_handler(void)
{
    ticks ++;
    if ((ticks % 1000) == 0)
    {
        // char buff [50];
        // print_string("current ticks: ");
        // print_string(uint32_to_str(ticks, buff));
        // print_string("\n");
    }
    // Logic: Switch every 8 ticks
    if (ticks % 1 == 0) {
        schedule_next_task();
        // do_sched = true;
    }
}



__attribute__((naked))
void timer_isr(void) {
    asm volatile(
        "pusha\n\t"

        // Send EOI IMMEDIATELY so other interrupts (keyboard) can fire
        "movb $0x20, %%al\n\t"
        "outb %%al, $0x20\n\t"
        "outb %%al, $0xA0\n\t"

        "mov 32(%%esp), %%eax\n\t"
        "push %%eax\n\t"
        "call timer_handler\n\t"
        "add $4, %%esp\n\t"

        "popa\n\t"
        "add $4, %%esp\n\t"
        "iret\n\t"
        : : : "memory"
    );
}




__attribute__((naked))
void context_switch(Task* next_task) {
    asm volatile(
        "pushal\n\t"              // 1. Save current task's state
        "movl running_task, %eax\n\t"
        "movl %esp, (%eax)\n\t"   // 2. Save current ESP into running_task->esp

        "movl 36(%esp), %eax\n\t" // 3. Get 'next_task' argument (offset 36 because of pushal + return address)
        "movl %eax, running_task\n\t" // 4. Update running_task pointer
        "movl (%eax), %esp\n\t"   // 5. Load the NEW task's ESP

        "popal\n\t"               // 6. Restore new task's state
        "ret\n\t"                 // 7. Jump to new task's saved EIP
    );
}


void schedule_next_task(void) {
    int next_index = (current_index + 1) % task_count;
    if (next_index == current_index) return; // Don't switch to yourself

    Task* next = &task_list[next_index];
    current_index = next_index;
    
    context_switch(next);
}



void task_starter(void (*func)()) {
    __asm__ volatile("sti"); // Crucial: Re-enable interrupts for the new task
    func();
    while(1); 
}

void create_task(void (*func)()) {
    uint32_t raw = (uint32_t)malloc(4096 + 32);
    uint32_t* stack = (uint32_t*)((raw + 4096) & ~0xF);

    *(--stack) = (uint32_t)func;         // Argument for task_starter
    *(--stack) = 0;                      // Dummy return address
    *(--stack) = (uint32_t)task_starter; // context_switch returns to here
    
    for(int i=0; i<8; i++) *(--stack) = 0; // pushal frame

    task_list[task_count].esp = (uint32_t)stack;
    task_count++;
}

void sleep(uint16_t ms)
{
    uint32_t start_ms = ticks;
    while (start_ms + (ms) > ticks) ;
}