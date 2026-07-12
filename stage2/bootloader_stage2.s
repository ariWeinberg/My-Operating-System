[bits 16]
; [org 0x8000]


main:
    ; extern map_memory

    [BITS 16]
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    call map_memory

    ; now switch to protected/long mode

    cli                     ; no interrupts
    call enable_a20

    lgdt [gdt_descriptor]   ; load GDT

    mov eax, cr0
    or  eax, 1
    mov cr0, eax             ; set PE bit

    jmp CODE_SEL:pm_entry    ; FAR jump (flush pipeline)




jmp end


enable_a20:
    in   al, 0x92
    or   al, 00000010b
    out  0x92, al
    ret


end:

    cli
    hlt
    jmp end

; start vars
message_helloWorldStage2:
db "hello world - stage 2. yay!!!", 0x0A, 0x0D, 0
; end vars

; start gdt
gdt_start:
gdt_null:
    dq 0x0000000000000000

gdt_code:
    dq 0x00CF9A000000FFFF   ; base=0, limit=4GB, code, ring 0

gdt_data:
    dq 0x00CF92000000FFFF   ; base=0, limit=4GB, data, ring 0

gdt_end:


gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start



CODE_SEL equ gdt_code - gdt_start
DATA_SEL equ gdt_data - gdt_start

; CODE_SEL equ 0x08
; DATA_SEL equ 0x10

; end gdt


%include "mem_map.s"

[bits 32]
extern c_main
pm_entry:

    mov ax, DATA_SEL
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov esp, 0x90000



    mov dl, [boot_drive]  ; restore boot drive number
    call c_main

    jmp hang


    
;--------------------------------------------
; void print_string_c(const char *s)
; cdecl: argument on stack, caller cleans
;--------------------------------------------
hang:
    cli
    hlt
    jmp hang

times 1022 - ($ - $$) db 0


boot_drive: equ 0x7DFD
db 0x66, 0xBB
