BITS 16
ORG 0x0000

start:
    ; Set up segments
    xor ax, ax
    mov ds, ax
    mov es, ax

    ; Set up stack
    mov ss, ax
    mov sp, 0x7C00

    ; Print message
    mov si, hello_msg
    call print_string

    ; Hang the system
hang:
    hlt
    jmp hang

print_string:
    lodsb
    test al, al
    jz .done
    mov ah, 0x0E
    int 0x10
    jmp print_string
.done:
    ret

hello_msg db 'Hello, Kernel loaded successfully!', 0

TIMES 512-($-$$) db 0
;;DW 0xAA55
