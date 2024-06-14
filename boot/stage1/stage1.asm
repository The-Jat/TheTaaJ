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

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;; Debuggin Purpose
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;; Print the Register value in hex and decimal
	; mov ax, 1234
	; mov dx, ax
	; call PrintWordHex	; Print dx value in hex
	; call PrintNewline	; \n
	; call PrintWordNumber	; Print ax value in number
	; call PrintNewline	; \n

	;Print Welcome to the Screen
	mov si, WelcomeToStage1		; Load the address of the string into si register
	call PrintString16BIOS		; String printing function.
	call PrintNewline		; \n

	; Load stage from the disk
	mov dl, [bPhysicalDriveNum]	; Drive number
	mov ch, 0		; Cylinder number
	mov dh, 0		; Head number
	mov cl, 2		; Sector starting (Indexed 1, as first sector is at index 1)
	mov ax, 0x0000
	mov es, ax
	mov bx, STAGE_2_LOAD_ADDRESS		; Memory address (0x500)
	mov al, STAGE_2_SECTORS_COUNT	; 58 Number of sectors to read
	call ReadFromDisk	; Call the routine to read from disk

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;; Pass Data from stage 1 to stage 2,
	;; through register
	; mov ax, 77
	; Call PrintWordNumber
	; call PrintNewline	; \n
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;; Pass Data from stage 1 to stage 2,
	;; through known fixed memory location
	;; For our example, we will pass the data by storing
	;; at memory "0x7E00" which is just after the bootcode.
	; mov ax, 1628
	; mov word [0x7E00], ax		; Store the passing value at the location,
					; by specifying size explicitly.
	;; OR
	;; mov [0x7E00], ax		; Store the passing value at the location
					; Without specifying size explicitly, the assembler
					; interprets it as to store the data of size of AX
					; which is word size.
	;; These both methods works, as AX is of word size,
	;; so assembler only stores word size data at the location.
	; call PrintWordNumber		; Print the Passing data
	; call PrintNewline		; \n
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;; Pass Data from stage 1 to stage 2 Using Stack
	;; We will push the data in our stage 1
	;; and pop the data from in our stage 2
	; mov ax, 107	; Set AX to the value want to pass by pushing
	; call PrintWordNumber	; Print the passing value
	; call PrintNewline	; \n
	
	; push ax		; Push AX, which is 107 on the stack
	
	; mov ax, 108	; Change AX to 108
	; call PrintWordNumber	; Print Changed value
	; Call PrintNewline	; \n
	
	; push ax		; Push AX, which is 108 on the stack
	
	;; Here we have passed the value 107 and 108 in the stack,
	;; such that stack is:
	;;	|	| Top (Low Memory Area)
	;;	|  108	|
	;;	|-------|
	;;	|  107	| Bottom (High Memory Area)
	;;	---------
	;; At receiving end they will be received in reverse order.
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;; Pass drive number to stage 2
	;; we will pass it in register AL, fast and easy
	;; method for small data
	xor ax, ax		; for printing clear complete AX
	mov al, [bPhysicalDriveNum]	; put drive number in al to be passed.
	mov si, sPassedDriveNumber
	call PrintString16BIOS
	call PrintWordNumber	; Print the passing drive number
	call PrintNewline	; \n
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	; jump to the stage 2 land
	jmp STAGE_2_LOAD_ADDRESS	; 0x0500


	; Infinite loop
	jmp $


; **************************
; Variables
; **************************
bPhysicalDriveNum	db	0	; Define variable to store disk number	

WelcomeToStage1	db 'Welcome to the Stage1', 0	; Define welcome message
sPassedDriveNumber db	'Passed Drive Number from Stage1 : ', 0

; Fill out bootloader
times 510-($-$$) db 0		; Fill up the remaining space with zeroes

; Boot Signature
db 0x55, 0xAA		; Boot signature indicating valid bootloader
