/* ENTRY(main) */

SECTIONS
{
    /* Stage2 load address */
    . = 0x8000;

    /* 16-bit real-mode + transition code */
    .text16 :
    {
        build/bootloader_stage2.o(.text*)
    }

    /* Align before 32-bit code */
    . = ALIGN(16);

    /* 32-bit protected-mode C code */
    .text32 :
    {
        build/kernel.o(.text*)
    }

    .rodata :
    {
        *(.rodata*)
    }

    .data :
    {
        *(.data*)
    }

    .bss :
    {
        *(COMMON)
        *(.bss*)
    }
}
