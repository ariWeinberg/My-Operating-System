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


void k_create_task(void (*func)());
void k_sleep(uint16_t ms);
void schedule_next_task(void);
void timer_handler(void);
void timer_isr(void);
static void k_main(void);
static void k_init(void);
static void k_loop(void);
static void k_panic(void);
static void k_die(void);
static void k_setup_tasks(void);


void task_a(void);
void task_b(void);
void task_c(void);



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
}

static void k_main(void)
{
    clear_screen();
    print_string("hello from kernel!\n");
    k_setup_tasks();
    k_loop();
}

static void k_loop(void)
{
    while(1)
    {
        if(current_char_out < current_char_in)
        {
            put_char(char_buffer[current_char_out++], 0x0F);
        }
        // if (k_ticks % 1 == 0)
        // {
        //     schedule_next_task();
        // }
    }
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
    k_create_task(&task_a);
    k_create_task(&task_b);
    k_create_task(&task_c);
}

void k_sleep(uint16_t ms)
{
    uint32_t start_ms = k_ticks;
    while (start_ms + (ms) > k_ticks) ;
}





void task_a(void) {while(1){print_string("A"); k_sleep(500);}}
void task_b(void) {while(1){print_string("B"); k_sleep(2000);}}
void task_c(void) {while(1){print_string("C"); k_sleep(100);}}
