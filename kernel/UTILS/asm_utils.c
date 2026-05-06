#include "asm_utils.h"


void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}
uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}
uint16_t inw(uint16_t port) {
    uint16_t ret;
    __asm__ volatile ("inw %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}


/**
 * div64_32: Performs (uint64_t / uint32_t)
 * @dividend: The 64-bit value to divide (EDX:EAX)
 * @divisor:  The 32-bit value to divide by
 * @remainder: Pointer to store the remainder (EDX)
 * @return: The 32-bit quotient (EAX)
 */
uint32_t div64_32(uint64_t dividend, uint32_t divisor, uint32_t *remainder)
{
    uint32_t quotient;
    uint32_t rem;
    uint32_t high = (uint32_t)(dividend >> 32);
    uint32_t low  = (uint32_t)dividend;

    __asm__ volatile (
        "divl %4"
        : "=a"(quotient), "=d"(rem)   // EAX = quotient, EDX = remainder
        : "a"(low), "d"(high), "rm"(divisor)
        : "cc" // Notifies compiler that flags (condition codes) are modified
    );

    if (remainder) *remainder = rem;
    return quotient;
}


/**
 * idiv64_32: Performs (int64_t / int32_t) signed
 */
int32_t idiv64_32(int64_t dividend, int32_t divisor, int32_t *remainder)
{
    int32_t quotient;
    int32_t rem;
    int32_t high = (int32_t)(dividend >> 32);
    int32_t low  = (int32_t)dividend;

    __asm__ volatile (
        "idivl %4"
        : "=a"(quotient), "=d"(rem)
        : "a"(low), "d"(high), "rm"(divisor)
        : "cc"
    );

    if (remainder) *remainder = rem;
    return quotient;
}



// GCC internal: Unsigned 64-bit division
uint64_t __udivdi3(uint64_t dividend, uint64_t divisor) {
    return div64_32(dividend, (uint32_t)divisor, 0);
}

// GCC internal: Unsigned 64-bit modulo
uint64_t __umoddi3(uint64_t dividend, uint64_t divisor) {
    uint32_t remainder;
    div64_32(dividend, (uint32_t)divisor, &remainder);
    return (uint64_t)remainder;
}
void io_wait()
{
    
}