#ifndef KERNEL
#define KERNEL

#include "STD/bool.h"
#include "STD/int.h"
#include "STD/defs.h"

void k_create_task(void (*func)());
void k_sleep(uint16_t ms);
void timer_handler(void);
void timer_isr(void);
static void k_main(void);
static void k_init(void);
static void k_loop(void);
static void k_panic(void);
static void k_die(void);
static void k_setup_tasks(void);

#endif