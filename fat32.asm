BITS 16
ORG 0x7C00


start:
    cli
    xor ax, ax
    mov ss, ax
    mov sp, 0x7E00
    sti

    ; Set data segment registers
    mov ds, ax
    mov es, ax

    ; Print a message
    mov si, boot_msg
    call print_string
jmp $
    ; Load FAT32 BPB (BIOS Parameter Block)
    mov bx, 0x7E00
    mov dl, [boot_drive]
    call read_sectors

    ; Extract necessary BPB fields
    mov ax, [bx + BPB_BytsPerSec]
    mov [BytesPerSector], ax
    mov ax, [bx + BPB_SecPerClus]
    mov [SectorsPerCluster], ax
    mov eax, [bx + BPB_RsvdSecCnt]
    mov [ReservedSectors], eax
    mov eax, [bx + BPB_NumFATs]
    mov [NumberOfFATs], eax
    mov eax, [bx + BPB_FATSz32]
    mov [SectorsPerFAT], eax
    mov eax, [bx + BPB_RootClus]
    mov [RootCluster], eax

    ; Find the kernel file "KERNEL.BIN"
    call find_kernel_file

    ; Load the kernel into memory
    call load_kernel

    ; Jump to the kernel
    jmp 0x2000:0x0000

disk_error:
    hlt

; Function to read sectors using BIOS interrupt 0x13
read_sectors:
    push ax
    push dx
    push cx
    push bx

    mov ah, INT13_READ_SECTORS
    mov al, 0x01
    int 0x13
    jc disk_error

    pop bx
    pop cx
    pop dx
    pop ax
    ret

; Function to find "KERNEL.BIN" in the root directory
find_kernel_file:
    ; Set up the root directory reading
    mov eax, [RootCluster]
    call cluster_to_lba
    mov bx, 0x7E00
    call read_sectors

    ; Find the file
    mov di, 0x7E00
.find_loop:
    mov cx, 11
    mov si, kernel_name
    repe cmpsb
    je .file_found
    add di, 32
    cmp di, 0x7E00 + 512
    jb .find_loop

    jmp disk_error

.file_found:
    ; Get the starting cluster of the kernel file
    mov ax, [di + DIR_FstClusLO]
    mov dx, [di + DIR_FstClusHI]
    shl dx, 16
    or eax, edx
    mov [KernelCluster], eax
    ret

; Function to load the kernel file into memory
load_kernel:
    mov eax, [KernelCluster]
    mov bx, 0x2000
.load_loop:
    call cluster_to_lba
    call read_sectors
    add bx, 512
    call next_cluster
    test eax, eax
    jnz .load_loop
    ret

; Convert cluster number to LBA
cluster_to_lba:
    push ax
    push dx
    sub eax, 2
    mul word [SectorsPerCluster]
    add eax, [ReservedSectors]
    add eax, [SectorsPerFAT]
    add eax, [SectorsPerFAT]
    pop dx
    pop ax
    ret

; Get the next cluster in the chain
next_cluster:
    push ax
    push dx
    mov ebx, eax
    mov ax, 0
    mov al, byte [SectorsPerCluster]
    mul bx
    add eax, [FATStart]
    call read_sectors

    ; Compute the offset in the FAT
    mov eax, [KernelCluster]
    mov ecx, eax
    shr ecx, 2
    add ecx, [FATStart]
    call read_sectors

    mov ebx, eax
    and eax, 0x03
    shl eax, 2
    mov eax, dword [ebx + ebx]
    pop dx
    pop ax
    ret

; Print string function
print_string:
    lodsb
    test al, al
    jz .done
    mov ah, 0x0E
    int 0x10
    jmp print_string
.done:
    ret
; Define constants for BIOS interrupts
INT13_READ_SECTORS equ 0x02
BIOS_DISK_ERROR equ 0x01

boot_drive db 0
BytesPerSector dw 0
SectorsPerCluster db 0
ReservedSectors dw 0
NumberOfFATs db 0
SectorsPerFAT dd 0
RootCluster dd 0
KernelCluster dd 0

boot_msg db 'Loading kernel from FAT32...', 0
kernel_name db 'KERNEL  BIN'

%include 'fat32.inc'
; Boot sector signature
TIMES 510-($-$$) db 0
DW 0xAA55

