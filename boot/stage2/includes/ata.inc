%ifndef _STAGE_2_ATA_INC_
%define _STAGE_2_ATA_INC_

BITS 32  ;; 32 Bit Mode

; Define the I/O ports for the primary and secondary ATA channels
%define PRIMARY_ATA_DATA 0x1F0
%define SECONDARY_ATA_DATA 0x170

%define PRIMARY_ATA_FEATURES 0x1F1
%define SECONDARY_ATA_FEATURES 0x171

%define PRIMARY_ATA_SECTOR_COUNT 0x1F2
%define SECONDARY_ATA_SECTOR_COUNT 0x172

%define PRIMARY_ATA_SECTOR_NUMBER 0x1F3
%define SECONDARY_ATA_SECTOR_NUMBER 0x173

%define PRIMARY_ATA_CYLINDER_LOW 0x1F4
%define SECONDARY_ATA_CYLINDER_LOW 0x174

%define PRIMARY_ATA_CYLINDER_HIGH 0x1F5
%define SECONDARY_ATA_CYLINDER_HIGH 0x175

%define PRIMARY_ATA_DRIVE_OR_HEAD 0x1F6
%define SECONDARY_ATA_DRIVE_OR_HEAD 0x176

%define PRIMARY_ATA_COMMAND_AND_STATUS 0x1F7
%define SECONDARY_ATA_COMMAND_AND_STATUS 0x177

%define MASTER 0xA0
%define SLAVE 0xB0

%define IDENTIFY_DEVICE_COMMAND 0xEC


; ********************************
; identify_ata_devices = This function calls the identify_device function
; 	for all four possible ATA devices.
;	BX = 0, CX = 0	; Primary Channel, Master Device
;	BX = 0, CX = 1	; Primary Channel, Slave Device
;	BX = 1, CX = 0	; Secondary Channel, Master Device
;	BX = 1, CX = 1	; Secondary Channel, Slave Device
; ******************************** 
identify_ata_devices:
	pushad
	; Get device info of Primary Channel, Master Device
	mov bx, 0	; Primary Channel (Bus)
	mov cx, 0	; Master Device
	call identify_device ; Get identification information

	; Get device info of Primary Channel, Slave Device
	mov bx, 0	; Primary Channel (Bus)
	mov cx, 1	; Slave Device
	call identify_device ; Get identification information
	
	; Get device info of Secondary Channel, Master Device
	mov bx, 1	; Secondary Channel (Bus)
	mov cx, 0	; Master Device
	call identify_device ; Get identification information

	; Get device info of Secondary Channel, Slave Device
	mov bx, 1	; Secondary Channel (Bus)
	mov cx, 1	; Slave Device
	call identify_device ; Get identification information

	popad
ret


; ********************************
; identify_device = This function detects the ATA device and prints its information.
; IN: 
;   - BX: 0 = PRIMARY CHANNEL, 1 = SECONDARY CHANNEL
;   - CX: 0 = MASTER DEVICE, 1 = SLAVE DEVICE
; ******************************** 
identify_device:
	pushad	; save cpu state

	;; Test for the Primary or Secondary Channel
	test bx, 0
	jne .secondary_channel
	
	;;  Primary Channel (bus)

	; Test for the Primary - Master or Slave
	test cx, 0
	jne .primary_slave_device

	;; Primary Master Device
	mov dx, PRIMARY_ATA_DRIVE_OR_HEAD	; Primary drive/head
	mov al, MASTER		; Select master device
	out byte dx, al
	
	; Jump to identify device command
	jmp .identify_device

	;; Primary Slave Device
	.primary_slave_device:
	mov dx, PRIMARY_ATA_DRIVE_OR_HEAD	; Primary drive/head
	mov al, SLAVE
	out byte dx, al		; Select slave device
	
	; Jump to identify device command
	jmp .identify_device

.secondary_channel:

	;; Secondary Channel (bus)
	test cx, 0
	jne .secondary_slave_device
	;; Secondary master device
	
	mov dx, SECONDARY_ATA_DRIVE_OR_HEAD
	mov ax, MASTER	; Secondary drive/head
	out byte dx, ax	; Select master device
	
	; Jump to identify device command
	jmp .identify_device

	;; Secondary slave device
	.secondary_slave_device:
	mov ax, SLAVE	; Primary drive/head
	mov dx, SECONDARY_ATA_DRIVE_OR_HEAD
	out byte dx, ax		; Select master device
	; Jump to identify device command
	jmp .identify_device

.identify_device:
	test cx, 0
	jne .slave_command

	;; For Master Command
	mov dx, PRIMARY_ATA_COMMAND_AND_STATUS
	mov al, IDENTIFY_DEVICE_COMMAND
	out byte dx, al
	
	jmp .poll_device_master_slave

	;; For Slave Command
	.slave_command:
	mov dx, SECONDARY_ATA_COMMAND_AND_STATUS
	mov ax, IDENTIFY_DEVICE_COMMAND
	out byte dx, al
	
	jmp .poll_device_master_slave
	
	.poll_device_master_slave:
	test cx, 0
	jne .poll_device_slave
	
	mov dx, PRIMARY_ATA_COMMAND_AND_STATUS
	jmp .poll_device
	
	.poll_device_slave:
	mov dx, SECONDARY_ATA_COMMAND_AND_STATUS
	jmp .poll_device

.poll_device:
	.wait_status:
	in byte al, dx
	test al, 0x80	; Check BSY bit (bit 7 with zero-indexing)
	jnz .wait_status	; Wait until BSY bit is clear

	test al, 0x01		; Check ERR bit (bit 0)
	jnz .no_device
	
	test al, 0x08		; Check DRQ bit (bit 3)
	jz .no_device


	; Read 256 words (512 bytes) of identification
	; data from Data Register
	test bx, 0
	jne .secondary_channel_data

	mov dx, PRIMARY_ATA_DATA
	jmp .start_reading

	.secondary_channel_data:
	mov dx, SECONDARY_ATA_DATA
	jmp .start_reading

.no_device:
	; Print "No Device" message
	mov esi, no_device_message
	mov ecx, 9  ; Length of "No Device"
	call print_fixed_size_string
	jmp .end

.start_reading:

	mov di, identifier_data
	mov cx, 256
	rep insw
	
	; Extract Serial Number (words 10-19)
	mov si, identifier_data + 20  ; Offset to Serial Number
	mov di, serial_number
	mov cx, 20
	call copy_and_swap_bytes

	; Print the serial number
	mov esi, serial_number
	mov ecx, 20		; Size of string
	call print_fixed_size_string

	; Extract Firmware Revision (words 23-26)
	mov si, identifier_data + 46  ; Offset to Firmware Revision
	mov di, firmware_revision
	mov cx, 8
	call copy_and_swap_bytes

	; Print the firmware revision
	mov esi, firmware_revision
	mov ecx, 8
	call print_fixed_size_string


	; Extract Model Number (words 27-46)
	mov si, identifier_data + 54  ; Offset to Model Number
	mov di, model_number
	mov cx, 40
	call copy_and_swap_bytes
	
	; Print the model number
	mov esi, model_number
	mov ecx, 40
	call print_fixed_size_string

.end:
	popad
ret


; ********************************
; print_fixed_size_string: Fixed Size String Printing Function
; It retains the line number and starts printing from the next line in subsequent calls.
;
; IN: 
;   - ESI: Pointer to String,
;   - ECX: Size of the String
; ******************************** 
print_fixed_size_string:
	pushad
	mov edi, [video_memory]		; 0xB8000

	;; This code helps in starting from the next line every time,
	;; we call this function, but we have to increment the line every time.
	; al = line, initially 0
	; dl = 80*2 	; bytes in a single line
	; al = al * dl 	; offset to the next line
	mov byte al, [cursor_line]
	mov dl, single_line_vmem_bytes	; Number of bytes in a single line 80*2
	mul dl			; multiply with the al
	add word edi, eax	; add the offset in the video_memory base

	.print_next_char:
		mov byte bl, [esi]	; Load first char in bl
		mov byte [edi], bl	; Load char from bl to video memory's first byte
		mov byte [edi + 1], 0x05	; Load the color to the next byte of vMem.
		dec ecx			; Decrement string counter
		cmp ecx, 0		; Check if it was the last char of string.
		jz .done_print		; jump to .done_print, if it was the last char

		add edi, 2	; increment the vmem by 2, point to next box of vMem
		add esi, 1	; point to next char in string.
		jmp .print_next_char	; loop to print the next char
	.done_print:		; it was the end of string.
		inc byte [cursor_line]		; increment the cursor_line and save in variable
	popad
ret
; ********************************


; Function to copy and swap bytes
copy_and_swap_bytes:
    cld
.copy_loop:
    lodsw                    ; Load word from [DS:SI] into AX
    xchg al, ah              ; Swap the two bytes in AX
    stosw                    ; Store swapped word to [ES:DI]
    loop .copy_loop
ret

section .data
;; Video Memory address of 80*25 (mode 3)
video_memory 	dd 	0xB8000

cursor_line db 0

identifier_data dw 256 dup(0)	; Buffer to store identification data (512 bytes)
serial_number db 20 dup(0)      ; Buffer to store the serial number
firmware_revision db 8 dup(0)   ; Buffer to store the firmware revision
model_number db 40 dup(0)       ; Buffer to store the model number

single_line_vmem_bytes equ 80*2	; Total bytes in a single line in 80*25 (Mode 3)
				; 80 boxes and each box is of 2 bytes

no_device_message db "No Device"

%endif
