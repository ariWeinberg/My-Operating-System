#include "screen_driver.h"
#include "../STD/int.h"

uint32_t video_memory_base = 0xB8000;

const uint8_t columns = 80;
const uint8_t rows = 25;
const uint8_t tab_stop = 8;


uint8_t cursor_x = 0;
uint8_t cursor_y = 0;

void clear_screen()
{
    uint8_t *vmem = (uint8_t*)video_memory_base;

    for (int i = 0; i < rows * columns; i++) {
        vmem[i * 2]     = ' ';
        vmem[i * 2 + 1] = 0x00;
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

bool put_char(char c, uint8_t color)
{
    uint8_t *cur = (uint8_t*)(video_memory_base + ((cursor_y * columns + cursor_x) * 2));
    switch (c)
    {
    case 0x0:
        return false;
    case 10:
        cursor_x = 0;
        cursor_y = (cursor_y + 1) % rows;
        return true;
    case 13:
        cursor_x = 0;
        return false;
    case 8:
        if (cursor_x == 0 && cursor_y == 0)
            return false;

        if (cursor_x == 0)
        {
            cursor_y -= 1;
            cursor_x = columns - 1;
        }
        else
            cursor_x -= 1;

        cur = (uint8_t*)(video_memory_base + ((cursor_y * columns + cursor_x) * 2));
        cur[0] = ' ';
        cur[1] = color;
        return false;
    case 9: // Horizontal Tab (\t)
        // Calculate how many spaces to skip to reach the next multiple of 8
        uint8_t spaces = 8 - (cursor_x % 8);
        
        // Move the cursor forward
        cursor_x += spaces;

        // Handle line overflow if the tab pushes the cursor past the screen width
        if (cursor_x >= columns)
        {
            cursor_x = 0; // Or cursor_x % columns if you want it to wrap naturally
            cursor_y = (cursor_y + 1) % rows;
        }
        return cursor_x == 0;

    default:
        break;
    }
    cur[0] = c;
    cur[1] = color;

    cursor_x = (cursor_x + 1) % columns;
    if (cursor_x == 0)
    {
        cursor_y = (cursor_y + 1) % rows;
        return true;
    }

    return false;
}
