ENTRY(_start)

PHDRS
{
    text PT_LOAD FLAGS(5); /* R_X = 4 | 1 */
    data PT_LOAD FLAGS(6); /* R_W = 4 | 2 */
}

SECTIONS
{
    . = 0x100000;

    .text : {
        KEEP(*(.text.boot))
        *(.text*)
    } :text

    .rodata : {
        *(.rodata*)
    } :text

    .data : {
        *(.data*)
    } :data

    .bss : {
        *(COMMON)
        *(.bss*)
    } :data
}
