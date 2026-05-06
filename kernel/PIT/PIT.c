#include "PIT.h"
#include "../UTILS/asm_utils.h"

// Simulated Global State
static uint16_t pit_reload_value;
static uint32_t pit_irq0_frequency;
static uint32_t pit_irq0_ms;
static uint32_t pit_irq0_fractions;

void pit_init(uint32_t frequency_hz) {
    uint32_t reload;

    // 1. Bounds checking and Initial Reload Calculation
    if (frequency_hz <= 18) {
        reload = 0x10000; // 65536
    } else if (frequency_hz >= 1193181) {
        reload = 1;
    } else {
        // Logic: eax = (3579545 / frequency) / 3 with rounding
        // This is equivalent to: (1193181.666... / frequency)
        uint64_t dividend = 3579545;
        uint32_t quotient = (uint32_t)(dividend / frequency_hz);
        uint32_t remainder = (uint32_t)(dividend % frequency_hz);
        
        if (remainder >= (3579545 / 2)) quotient++;

        // Second stage division by 3
        reload = quotient / 3;
        if ((quotient % 3) >= 2) reload++; // 3/2 rounded is 1.5, so remainder 2+ rounds up
    }

    pit_reload_value = (uint16_t)reload;
    
    // 2. Calculate Actual Frequency from the final reload value
    // Logic: eax = (3579545 / reload) / 3 with rounding
    uint32_t freq_quotient = 3579545 / reload;
    uint32_t freq_rem = 3579545 % reload;
    if (freq_rem >= (3579545 / 2)) freq_quotient++;

    pit_irq0_frequency = freq_quotient / 3;
    if ((freq_quotient % 3) >= 2) pit_irq0_frequency++;

    // 3. Calculate 32.32 Fixed Point Time (ms)
    // Assembly: reload * 0xDBB3A062 >> 10
    uint64_t time_fixed = (uint64_t)0xDBB3A062 * reload;
    time_fixed >>= 10;

    pit_irq0_ms = (uint32_t)(time_fixed >> 32);
    pit_irq0_fractions = (uint32_t)time_fixed;

    // 4. Program PIT Hardware
    // Use [__builtin_ia32_cli](https://gcc.gnu.org) or similar for your compiler
    __asm__ volatile ("cli"); 

    // Channel 0, lobyte/hibyte, rate generator (0x34)
    outb(0x43, 0x34);
    outb(0x40, (uint8_t)pit_reload_value);        // Low Byte
    outb(0x40, (uint8_t)(pit_reload_value >> 8)); // High Byte

    __asm__ volatile ("sti");
}
