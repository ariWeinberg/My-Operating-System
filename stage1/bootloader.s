[bits 16]
[org 0x7c00]

jmp main
nop
; BPB stat
BPB_START:
OEM_IDENTIFIER: db '12345678'
BYTES_PER_SECTOR: dw 512
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
VOLUME_LABEL: db "0 123456789"
SYSTEM_IDENTIFIER: db "12345678"

EBR_END:


main:
    ; preserve boot drive number
    mov [boot_drive], dl

    ; reset all segment registers and setup stack
    mov ax, 0
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7A00

    ; clear screen
    call clear_screen_no_bios

    ; ; print hello message
    mov si, message_helloWorldNoBios
    call print_string_no_bios

    mov ax, 0x0000
    mov es, ax
    mov bx, 0x8000
    ; load next part
    mov dl, [boot_drive]
    call load_sectors

    mov si, sector_loaded_message
    call print_string_no_bios
    

    jmp 0x0000:0x8000


jmp end


load_sectors:
    push ax
    push bx
    push cx
    push dx
    push si

    mov si, 5          ; retry counter

    .load_retry:
        mov ah, 0x02        ; BIOS read sectors
        mov al, 50           ; number of sectors
        mov ch, 0           ; cylinder
        mov cl, 2           ; sector (starts at 1)
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
        call clear_screen_no_bios
        mov si, disk_error_msg
        call print_string_no_bios
        jmp $
            ; halt

    .done:
        pop si
        pop dx
        pop cx
        pop bx
        pop ax
        ret

clear_screen_no_bios:
    push ax
    push cx
    push di
    push es

    mov  ax, 0xB800
    mov  es, ax
    xor  di, di

    mov  ax, 0x0F20      ; AH = attribute, AL = space (' ')
    mov  cx, 80*25       ; number of characters

    .rep:
        stosw                ; write AX to ES:DI, DI += 2
        loop .rep

        pop  es
        pop  di
        pop  cx
        pop  ax
        ret

print_string_no_bios:
    push si
    push di
    push es

    mov  ax, 0xB800
    
    mov  es, ax
    ; --- calculate DI from cursor_position ---
    ; DI = (y * 80 + x) * 2
    mov ax, [cursor_position.y]  ; row
    mov bx, [cursor_position.x]  ; column
    mov di, ax
    imul di, 80                  ; multiply row by 80
    add di, bx                   ; add column
    shl di, 1                    ; each cell = 2 bytes (char + attribute)
    mov ah, 0x04                 ; color

    mov  ah, 0x04

    .loop:
        lodsb
        test al, al
        jz   .done
        cmp al, 0x0A
        jz .newline
        cmp al, 0x0D
        jz .carriage_return
    .print_char:
        stosw
        call advance_cursor
        jmp  .loop
    .newline:
        call advance_cursor_by_line
        jmp .loop
    .carriage_return:
        call return_carriage
        jmp .loop
    .done:
        pop  es
        pop  di
        pop  si
        ret

Cursor:
    VGA.Width equ 80

    .SetCoords:
    ; input bx = x, ax = y
    ; modifies ax, bx, dx

        mov dl, VGA.Width
        mul dl
        add bx, ax

    .SetOffset:
    ; input bx = cursor offset
    ; modifies al, dx

        mov dx, 0x03D4
        mov al, 0x0F
        out dx, al

        inc dl
        mov al, bl
        out dx, al

        dec dl
        mov al, 0x0E
        out dx, al

        inc dl
        mov al, bh
        out dx, al
        ret

advance_cursor:
    push ax
    push bx

    mov ax, [cursor_position.x]
    mov bx, [cursor_position.y]
    inc ax
    cmp ax, 80
    jl .done
    mov ax, 0
    inc bx
    cmp bx, 25
    jl .done
    mov bx, 0
    .done:
        mov [cursor_position.x], ax
        mov [cursor_position.y], bx

        mov ax, [cursor_position.y]
        mov bx, [cursor_position.x]
        call Cursor.SetCoords
        pop bx
        pop ax
        ret

advance_cursor_by_line:
    mov word [cursor_position.x], 0
    inc word [cursor_position.y]
    cmp word [cursor_position.y], 25
    jl .ok
    mov word [cursor_position.y], 0 ; simple scroll not implemented
    .ok:
        mov ax, [cursor_position.y]
        mov bx, [cursor_position.x]
        call Cursor.SetCoords
        ret


return_carriage:
    mov word [cursor_position.x], 0
    mov ax, [cursor_position.y]
    mov bx, [cursor_position.x]
    mov [cursor_position.y], ax
    mov [cursor_position.x], bx
    call Cursor.SetCoords
    ret




end:
    cli
    hlt
    jmp end

message_helloWorldNoBios:
    db "hello world", 0x0A, 0x0D, 0
disk_error_msg:
    db "Disk read error!", 0x0D,0x0A,0
sector_loaded_message:
    db "sector 2 loaded", 0x0D,0x0A,0


; times 505 - ($ - $$) db 0
cursor_position:
    .x: dw 0
    .y: dw 0

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