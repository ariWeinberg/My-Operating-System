#include "stdin.h"
#include "../IDT/ISRs/keyboard/keyboard.h"
#include "../MEMORY_MANAGMENT/memory_managment.h"
#include "../STD/int.h"
#include "../STD/defs.h"
#include "../tasks/tasks.h"
#include "SCREEN_DRIVER/screen_driver.h"
#include "../tasks/task.h"

#define STDIN_SIZE (1024u * 1024u)


WaitQueue stdin_waiters = {0};
WaitQueue stdin_line_waiters = {0};


static volatile char *const stdin = (volatile char *)0xD00000;
volatile uint32_t stdin_in_count = 0;
volatile uint32_t stdin_out_count = 0;
volatile uint32_t stdin_peek_count = 0;

volatile uint32_t new_line_count = 0;

void stdin_init()
{
    wait_queue_init(&stdin_waiters);
    wait_queue_init(&stdin_line_waiters);
}

void stdin_insert(char c)
{
    if (stdin_in_count < STDIN_SIZE)
    {
        if (c == '\0')
        {
            return;
        }
        stdin[stdin_in_count++] = c;
        if (c == '\n')
        {
            new_line_count++;
            scheduler_wake_one(&stdin_line_waiters);
        }
        scheduler_wake_one(&stdin_waiters);
    }
}

char read_char(void)
{
    while (stdin_out_count >= stdin_in_count)
    {
        scheduler_wait(&stdin_waiters);
    }
    char c = stdin[stdin_out_count++];
    if (c == '\n' && new_line_count > 0)
    {
        new_line_count--;
    }
    return c; 
}

char peek_char(void)
{
    while (stdin_peek_count >= stdin_in_count)
    {
        scheduler_wait(&stdin_waiters);
    }
    return stdin[stdin_peek_count++];
}

char* read_line(void)
{
    while (new_line_count < 1)
    {
        scheduler_wait(&stdin_line_waiters);
    }
    enter_critical();
    uint32_t out_count = stdin_out_count;
    uint32_t in_count = stdin_in_count;

    uint32_t len = 0;

    for (uint32_t i = 0; i < (in_count - out_count); i++)
    {
        if (stdin[out_count + i] == '\n')
        {
            len = i + 1;
            break;
        }
    }
    if (len == 0)
    {
        exit_critical();
        return NULL;
    }
    char *dst = malloc(len + 1);
    if (dst == NULL)
    {
        exit_critical();
        return NULL;
    }
    for (uint32_t i = 0; i < len; i++)
    {
        dst[i] = stdin[out_count + i];
    }
    dst[len] = '\0';
    if (new_line_count > 0)
    {
        new_line_count--;
    }
    stdin_out_count = out_count + len;
    exit_critical();
    return dst;
}

void stdin_loop(void)
{
    while(1)
    {
        if(current_char_out < current_char_in)
        {
            const char c = char_buffer[current_char_out++];
            put_char(c, 0x0F);
            stdin_insert(c);
        }
        else
        {
            asm volatile("pause");
        }
    }
}
