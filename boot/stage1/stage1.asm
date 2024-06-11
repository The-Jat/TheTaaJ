; 16 Bit Code, Origin at 0x0
BITS 16
ORG 0x7C00

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
	
	;Print Welcome to the Screen
	mov si, WelcomeToStage1		; Load the address of the string into si register
	call PrintString16BIOS		; String printing function.
	
	; Infinite loop
	jmp $




; ********************************
; PrintChar
; IN:
; 	- al: Char to print
; ********************************
PrintChar:
	pusha			; Save all registers

	; Setup INT
	mov 	ah, 0x0E	; Function to print character
	mov 	bx, 0x00	; Video page number
	int 	0x10		; BIOS interrupt for video services

	; Restore & Return
	popa			; Restore all saved registers
	ret			; Return from function

; ********************************
; PrintNumber
; IN:
; 	- EAX: NumberToPrint
; ********************************
PrintNumber:
	; Save state
	pushad			; Save all registers

	; Loops
	xor 	bx, bx		; Clear BX register
	mov 	ecx, 10		; Set loop counter

	.DigitLoop:
	    xor 	edx, edx	; Clear EDX register
	    div 	ecx		; Divide eax by 10

	    ; now eax <-- eax/10
	    ;     edx <-- eax % 10

	    ; print edx
	    ; this is one digit, which we have to convert to ASCII
	    ; the print routine uses edx and eax, so let's push eax
	    ; onto the stack. we clear edx at the beginning of the
	    ; loop anyway, so we don't care if we much around with it

	    ; convert dl to ascii
	    add 	dx, 48		; Convert digit to ASCII

	    ; Store it
	    push 	dx		; Push ASCII digit
	    inc 	bx		; Increment counter bx

	    ; if eax is zero, we can quit
	    cmp 	eax, 0		; Compare EAX with 0
	    jnz 	.DigitLoop	; If not zero, loop

	.PrintLoop:
		pop 	ax		; Pop ASCII digit from stack

		; Print it
		call 	PrintChar	; Print character

		; Decrease
		dec 	bx		; Decrease counter BX
		jnz 	.PrintLoop	; If not zero, loop

    ; Done
    popad	; Restore all saved registers
    ret		; Return from function
   

; ********************************
; Print String 16 BIOS
; Prints the string in real mode using BIOS function.
; IN:
; 	- ESI: Null Terminated String
; ********************************
PrintString16BIOS:
	pusha		; pusha all register on stack
	mov ah, 0x0E	; BIOS teletype function
.print_string_16_bios_loop:	; label for loop
	lodsb			; Load byte at [SI] into AL and increment SI
	test al, al		; Perform bitwise AND of AL with itself to set flags
	jz .print_string_16_bios_done	; If zero flag is set (AL was zero), jump to done
	int 0x10			; Call BIOS interrupt
	jmp .print_string_16_bios_loop	;  Repeat for the next character
.print_string_16_bios_done:		; Label for the printing done
	popa				; Pop all the saved registers
	ret				; return from the function

; **************************
; Variables
; **************************
bPhysicalDriveNum	db	0	; Define variable to store disk number	

WelcomeToStage1	db 'Welcome to the Stage1', 0	; Define welcome message

; Fill out bootloader
times 510-($-$$) db 0		; Fill up the remaining space with zeroes

; Boot Signature
db 0x55, 0xAA		; Boot signature indicating valid bootloader
