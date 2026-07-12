; memory_map.asm
[BITS 16]

global map_memory

MEMORY_MAP_COUNT_ADDR equ 0x5000
MEMORY_MAP_ADDR       equ 0x5004

map_memory:
    xor ebx, ebx
    xor bp, bp
    mov di, MEMORY_MAP_ADDR

.next:
    mov eax, 0xE820
    mov edx, 0x534D4150 ; 'SMAP'
    mov ecx, 24
    int 0x15

    jc .done

    cmp eax, 0x534D4150
    jne .done

    cmp ecx, 20
    jb .done

    inc bp
    add di, 24

    test ebx, ebx
    jne .next

.done:
    mov [MEMORY_MAP_COUNT_ADDR], bp
    ret