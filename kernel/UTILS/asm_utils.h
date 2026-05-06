#ifndef ASM_UTILS
#define ASM_UTILS
#include "../STD/int.h"


void outb(uint16_t port, uint8_t val);
uint8_t inb(uint16_t port);
uint16_t inw(uint16_t port);
void io_wait();

uint32_t div64_32(uint64_t dividend, uint32_t divisor, uint32_t *remainder);
int32_t idiv64_32(int64_t dividend, int32_t divisor, int32_t *remainder);

// GCC internal: Unsigned 64-bit division
uint64_t __udivdi3(uint64_t dividend, uint64_t divisor);
// GCC internal: Unsigned 64-bit modulo
uint64_t __umoddi3(uint64_t dividend, uint64_t divisor);
#endif
