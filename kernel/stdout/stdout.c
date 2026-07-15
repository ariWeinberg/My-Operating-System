#include "stdout.h"
#include "../tasks/tasks.h"
#include "../STD/int.h"
#include "../STD/bool.h"
#include "../STD/defs.h"
#include "../SCREEN_DRIVER/screen_driver.h"
#include "../MEMORY_MANAGMENT/memory_managment.h"

uint8_t screen_row_count = 25;
uint8_t screen_col_count = 80;
uint8_t pages = 10;

uint8_t *buffer;

uint32_t stream_end = 0;
uint32_t view_start = 0;

bool stdout_is_initialized = false;

void stdout_init(void)
{
    if (stdout_is_initialized)
        return;

    if (buffer != NULL)
        free(buffer);

    buffer = zalloc(screen_row_count * screen_col_count * pages);

    if (buffer != NULL)
    {
        stream_end = 0;
        view_start = 0;
        stdout_is_initialized = true;
    }
}

void stdout_write(char c)
{
    if (!stdout_is_initialized)
    return;

    enter_critical();
    uint32_t capacity = (uint32_t)screen_row_count * screen_col_count * pages;

    if (stream_end < capacity)
        buffer[stream_end++] = c;

    exit_critical();
}

void stdout_write_line(const char *s)
{
    if (!stdout_is_initialized)
    return;

    for (int i = 0; s[i]; i++)
        stdout_write(s[i]);

    stdout_write('\n');
}

void stdout_scroll_line(ScrollDirection direction)
{
if (!stdout_is_initialized)
          return;

      enter_critical();

      if (direction == DOWN)
      {
          while (view_start < stream_end && buffer[view_start++] != '\n')
              ;
      }
      else if (direction == UP && view_start > 0)
      {
          view_start--;
          while (view_start > 0 && buffer[view_start - 1] != '\n')
              view_start--;
      }

      stdout_flush();
      exit_critical();
}

void stdout_scroll_page(ScrollDirection d)
{
    if (!stdout_is_initialized)
        return;
}

void stdout_flush(void)
{
    if (!stdout_is_initialized)
        return;

    enter_critical();
    while (true)
    {
        clear_screen();

        uint8_t visible_rows = 0;
        uint32_t next = view_start;
        while (next < stream_end && visible_rows < screen_row_count)
        {
            if (put_char(buffer[next], 0x0F))
                visible_rows++;
            next++;
        }

        if (next >= stream_end || visible_rows < screen_row_count)
            break;

        while (view_start < stream_end && buffer[view_start++] != '\n')
            ;
    }

    exit_critical();
}

void stdout_loop(void)
{
    while (true)
    {
        stdout_flush();
        scheduler_sleep(90);
    }
}
