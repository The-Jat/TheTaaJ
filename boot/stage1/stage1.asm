; 16 Bit Code, Origin at 0x0
BITS 16
ORG 0x7C00

jmp main

; Includes
%include "boot/common/defines.inc"	; For constants and common variables
%include "boot/common/print16.inc"	; For printing functions
%include "boot/common/disk.inc"		; For disk read function

main:
	; Disable Interrupts, unsafe passage
	cli

	; Far jump to fix segment registers
	jmp 	0x0:FixCS

FixCS:
	; Fix segment registers to 0
	xor 	ax, ax
	mov	ds, ax
	mov	es, ax

	; Set stack
	; The sp register is used to point to the top of the stack. By setting sp to 0x7C00, the bootloader ensures that the stack starts at the top of the memory allocated for the bootloader. This is important because the stack grows downward in memory, so it's set up before any other code runs.
	mov	ss, ax
	mov	ax, 0x7C00	; It ensure that there's space for the stack to grow downward without overlapping with other code or any other data in memory.
	mov	sp, ax

	; set interrupts
	sti

	; Save the DL register value, which contains the disk number 
	mov 	byte [bPhysicalDriveNum], dl

	call ClearScreenAndResetCursor	; Clear the screen and reset the cursor

	; Print the Register value in hex and decimal
	mov ax, 1234
	mov dx, ax
	call PrintWordHex	; Print dx value in hex
	call PrintNewline	; \n
	call PrintWordNumber	; Print ax value in number
	call PrintNewline	; \n

	;Print Welcome to the Screen
	mov si, WelcomeToStage1		; Load the address of the string into si register
	call PrintString16BIOS		; String printing function.
	call PrintNewline		; \n

	; Load stage from the disk
	mov dl, 0x80;[bPhysicalDriveNum]	; Drive number
	mov ch, 0		; Cylinder number
	mov dh, 0		; Head number
	mov cl, 2		; Sector starting (Indexed 1, as first sector is at index 1)
	mov ax, 0x0000
	mov es, ax
	mov bx, STAGE_2_LOAD_ADDRESS		; Memory address (0x500)
	mov al, STAGE_2_SECTORS_COUNT	; 58 Number of sectors to read
	call ReadFromDisk	; Call the routine to read from disk

	; jump to the stage 2 land
	jmp STAGE_2_LOAD_ADDRESS	; 0x0500


	; Infinite loop
	jmp $


; **************************
; Variables
; **************************
bPhysicalDriveNum	db	0	; Define variable to store disk number	

WelcomeToStage1	db 'Welcome to the Stage1', 0	; Define welcome message

; Fill out bootloader
times 510-($-$$) db 0		; Fill up the remaining space with zeroes

; Boot Signature
db 0x55, 0xAA		; Boot signature indicating valid bootloader
