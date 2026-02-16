[bits 16]
[org 0x7c00]

jmp main
nop
; BPB stat
BPB_START:
OEM_IDENTIFIER: times 8 db 0
BYTES_PER_SECTOR: dw 0
SECTORS_PER_CLUSTER: db 0
RESERVED_SECTORS: dw 0
FAT_COUNT: db 0
ROOT_DIR_ENTRIES: dw 0
TOTAL_SECTORS: dw 0
MEDIA_DESCRIPTOR_TYPE: db 0
SECTORS_PER_FAT: dw 0
SECTORSPER_TRACK: dw 0
HEAD_COUNT: dw 0
HIDDEN_SECTORS_COUNT: dd 0
LARGE_SECTORS_COUNT: dd 0
BPB_END:

EBR_START:
DRIVE_NUMBER: db 0
FLAGS_RESERVED: db 0
SIGNATURE: db 0
VOLUME_ID: dd 0
VOLUME_LABEL: times 11 db 0
SYSTEM_IDENTIFIER: times 8 db 0

EBR_END:


main:
    ; preserve boot drive number
    mov [boot_drive], dl

    ; reset all segment registers and setup stack
    mov ax, 0x0000
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7A00

    ; clear screen
    call clear_screen

    ; es is already set to zero from before
    mov bx, 0x8000

    ; load next part
    ; mov dl, [boot_drive] | dl is already set by bios (and it is preserved)
    call load_sectors

    jmp 0x0000:0x8000


jmp end

clear_screen:
    pusha

    mov ah, 00h      ; Function 00h: Set video mode
    mov al, 03h      ; AL = 03h: 80x25 text mode, 16 colors
    int 10h          ; Call the BIOS video services interrupt

    popa
    ret

print_string:
    pusha
    mov ah, 0x0e                 ; function code

    .loop:
        lodsb
        test al, al
        jz   .done
    .print_char:
        int 0x10
        jmp  .loop
    .done:
        popa
        ret

load_sectors:
    pusha

    mov si, 5          ; retry counter

    .load_retry:
        mov ah, 0x02        ; BIOS read sectors
        ; mov al, 50           ; number of sectors
        mov al, [RESERVED_SECTORS]           ; number of sectors
        mov cx, 0x0002      ; cylinder 0, sector 2
        mov dh, 0           ; head
        ; dl is already set before call
        int 0x13
        jc  .error          ; jump if carry = error

        jmp .done           ; success

    .error:
        dec si
        jz .disk_fail
        jmp .load_retry

    .disk_fail:
        call clear_screen
        mov si, disk_error_msg
        call print_string
        jmp $ ; halt

    .done:
        popa
        ret


end:
    ; cli
    ; hlt
    jmp end

disk_error_msg:     db "Disk error!", 0x0D,0x0A,0


boot_drive: db 0

times 446 - ($ - $$) db 0

; Partition 1 (dummy, but valid)
db 0x80              ; bootable
db 0,2,0             ; CHS start (ignored)
db 0x83              ; type (Linux)
db 0,2,0             ; CHS end
dd 1                 ; LBA start
dd 100               ; sector count

times 16*3 db 0


db 0x55, 0xAA