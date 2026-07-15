#include "SCREEN_DRIVER/screen_driver.h"
#include "IDT/IDT.h"
#include "IDT/ISRs/exceptions/exceptions.h"
#include "IDT/ISRs/keyboard/keyboard.h"
#include "PIC/PIC.h"
#include "PIT/PIT.h"
#include "UTILS/utils.h"
#include "STD/bool.h"
#include "GDT/GDT.h"
#include "./MEMORY_MANAGMENT/memory_managment.h"
#include "./IDT/ISRs/timer/timer.h"
#include "./tasks/tasks.h"
#include "kernel.h"
#include "STRING/string.h"
#include "stdin/stdin.h"
#include "stdout/stdout.h"


void task_a(void);
void task_b(void);
void task_c(void);
void echo_task(void);



static void kernel_setup_interrupts(void);
__attribute__((section(".text.boot"))) 
void _start(void)
{
    k_init();
    k_main();
}

static void k_init(void)
{
    gdt_init();
    kernel_setup_interrupts();
    stdin_init();
    stdout_init();
}

static void k_main(void)
{
    clear_screen();
    stdout_write_line("hello from kernel!");
    init_tasks();
    k_setup_tasks();
    k_loop();
}

static void k_loop(void)
{
    while(false)
    {
        // const char c = peek_char();
        // put_char(c, 0x0F);
    }
    idle_task();
}

static void k_panic(void)
{
    k_die();
}

static void k_die(void)
{
    interrupts_disable();
    while(1)
    {
        asm volatile (
            "hlt"
        );
    }
}

static void kernel_setup_interrupts(void) {
    interrupts_disable();
    pic_remap(0x20, 0x28);
    idt_init();
    idt_ignore_all();
    setup_exceptions();
    irq_mask_all();
    init_keyboard_irq();
    init_timer_irq();

    pit_init(1000);
    interrupts_enable();
}

static void k_setup_tasks(void)
{
    k_create_and_register_task(&stdin_loop);
    // k_create_and_register_task(&echo_task);
    // k_create_and_register_task(&task_a);
    // k_create_and_register_task(&task_b);
    // k_create_and_register_task(&task_c);
    k_create_and_register_task(&stdout_loop);
}

void k_sleep(uint16_t ms)
{
    scheduler_sleep(ms);
}





void echo_task(void)
{
    stdout_write_line("enter lines and i will echo them...");
    while (1)
    {
        const char *str = read_line();
        if (str == NULL)
        {
            stdout_write_line("oopsi... an error occourd.");
            k_panic(); // later add error messages...
            return;
        }
        if (!strcmp(str, "\n\0"))
        {
            continue;
        }
        stdout_write_line(str);
    }
    
}
void task_a(void) {while(1){stdout_write('A'); k_sleep(5000);}}
void task_b(void) {while(1){stdout_write('B'); k_sleep(20000);}}
void task_c(void) {while(1){stdout_write('C'); k_sleep(1000);}}
