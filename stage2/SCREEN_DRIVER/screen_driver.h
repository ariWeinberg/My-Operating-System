#ifndef SCREEN_DRIVER
#define SCREEN_DRIVER
#include "../STD/int.h"

void put_char(char c, uint8_t color);
void print_string(const char *s);
void clear_screen();
#endif
