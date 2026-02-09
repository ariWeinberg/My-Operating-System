ENTRY(_start) /* Tells the linker the entry point is the symbol _start */

SECTIONS
{
    . = 0x100000; /* Set the load address (e.g., 1MB) */

    .text : {
        KEEP(*(.text.boot)) /* Force the boot section to be first */
        *(.text)            /* All other code follows */
    }

    .data : { *(.data) }
    .bss  : { *(.bss)  }
}