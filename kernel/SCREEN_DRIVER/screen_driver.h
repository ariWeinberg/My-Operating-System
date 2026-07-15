#ifndef SCREEN_DRIVER_H
#define SCREEN_DRIVER_H
#include "../STD/int.h"
#include "../STD/bool.h"

bool put_char(char c, uint8_t color);
void print_string(const char *s);
void clear_screen();
#endif
