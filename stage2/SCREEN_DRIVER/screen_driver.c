#include "screen_driver.h"
#include "../STD/int.h"

uint32_t video_memory_base = 0xB8000;

uint8_t columns = 80;
uint8_t rows = 25;


uint8_t cursor_x = 0;
uint8_t cursor_y = 0;

void clear_screen()
{
    for(int i = 0; i < rows * columns; i++)
    {
        put_char(" "[0], 0x00);
    }
    cursor_x = 0;
    cursor_y = 0;
}

void print_string(const char *s)
{
    int i = 0;
    while (s[i] != 0)
    {
        put_char(s[i], 0x04);  // red text
        i++;
    }
}

void put_char(char c, uint8_t color)
{
    uint8_t *cur = (uint8_t*)(video_memory_base + ((cursor_y * columns + cursor_x) * 2));
    switch (c)
    {
    case 0x0:
        return;
    case 10:
        cursor_y = (cursor_y + 1) % rows;
        return;
    case 13:
        cursor_x = 0;
        return;
    
    default:
        break;
    }
    cur[0] = c;
    cur[1] = color;

    cursor_x = (cursor_x + 1) % columns;
    if (cursor_x == 0)
    {
        cursor_y = (cursor_y + 1) % rows;
    }
}
