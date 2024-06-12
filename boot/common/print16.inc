%ifndef _COMMON_PRINT_16_INC_
%define _COMMON_PRINT_16_INC_

; 16 Bit Code
BITS 16

; ********************************
; ClearScreenAndResetCursor
; This function clears the screen and resets the cursor at default (0,0) position
;*********************************
ClearScreenAndResetCursor:

	pusha
	mov ah, 0x06		; BIOS Function: scroll up
	mov al, 0		; Number of lines to scroll (0 = clear entire screen)
	mov bh, 0x07		; Attribute for blank lines (light gray on black)
	mov cx, 0x0000		; CH = 0, CL = 0 (upper left corner)
	mov dx, 0x184F		; End at the bottom right (row 24, column 79)
	int 0x10		; Call BIOS interrupt
	
	; Reset cursor position to the top-left corner
	mov ah, 0x02		; BIOS function: Set cursor position
	mov bh, 0x00		; Page number (0)
	mov dh, 0x00		; Row (0)
	mov dl, 0x00		; Column (0)
	int 0x10		; Call BIOS interrupt

	popa
ret


; ********************************
; PrintChar16BIOS
; This function prints the char on the screen stored in AL register using the BIOS interrupts.
; IN:
; 	- AL: Char
; ********************************
PrintChar16BIOS:
	; Save the state of AX and BX registers
	push 	ax
	push 	bx

	; Setup for BIOS teletype output (INT 0x10, AH = 0x0E)
	mov 	ah, 0x0E               ; BIOS teletype function (TTY output)
	mov 	bl, byte [bTextColor]  ; Load the text color into BL
	mov 	bh, 0x00               ; Set the page number to 0 (usually not needed for TTY output)
	int 	0x10                   ; BIOS video interrupt to print the character in AL

	; Restore the state of BX and AX registers
	pop 	bx
	pop 	ax
ret                          ; Return from the PrintChar16BIOS routine


; ********************************
; PrintString16BIOS

; This function prints the string referenced by the SI register using BIOS interrupts.
; IN:
; 	- SI: NULL-Terminated String
; ********************************
PrintString16BIOS:
	; Save all general-purpose registers
	pushad

	; Loop to process each character in the string
	.cLoop16BIOS:
		; Load the next byte from the string (pointed to by SI) into AL
		lodsb

		; Check if the loaded byte is NULL (end of string)
		or	al, al
		jz	.PrintDone  ; If AL is zero, jump to the end of the print routine

		; Print the character in AL
		call PrintChar16BIOS
		jmp	.cLoop16BIOS  ; Repeat the loop for the next character

	.PrintDone:
		; \n
		; Move cursor to the beginning of the next line (equivalent of \n)
		mov ah, 0x02	; BIOS function: Set Cursor position
		inc dh		; Move cursor down one line
		mov dl, 0x00	; Move cursor to the beginning of the line (column 0)
		int 0x10	; Set cursor position to (DL, DH)

		; Restore all general-purpose registers
		popad
ret  ; Return from the PrintString16BIOS routine


; ********************************
; PrintNumber
; IN:
; 	- EAX: NumberToPrint
; ********************************
PrintNumber:
	; Save all general-purpose registers
	pushad

	; Initialize variables
	xor 	ebx, ebx        ; Clear EBX to use it as a counter for the number of digits
    mov 	ecx, 10         ; Set ECX to 10, the divisor for converting number to digits

	.DigitLoop:
	    xor 	edx, edx        ; Clear EDX before division
	    div 	ecx             ; Divide EAX by 10
	    ; After div: EAX contains quotient, EDX contains remainder

	    ; Convert remainder to ASCII
	    add 	edx, 48         ; Convert digit to ASCII ('0' = 48)

	    ; Store ASCII digit on the stack
	    push 	edx
	    inc 	ebx             ; Increment digit count

	    ; If quotient (EAX) is zero, we're done converting digits
	    cmp eax, 0
	    jnz .DigitLoop       ; If EAX is not zero, repeat the loop

	.PrintLoop:
		; Pop ASCII digit from stack into EAX
		pop 	eax

		; Print the character in EAX
		call 	PrintChar16BIOS

		; Decrease digit count in EBX
		dec 	ebx
		jnz 	.PrintLoop     ; If EBX is not zero, print next digit

    ; Restore all general-purpose registers
    popad
ret                       ; Return from the PrintNumber routine


; ********************************
; SetTextColor
; IN:
; 	- AL: Background Color
;	- AH: Text Color
; ********************************
SetTextColor:
	; Save the state of AX and BX registers
	push 	ax
	push 	bx

	; Pack AH (Text Color) and AL (Background Color) into BL
	mov 	bl, ah          ; Move the text color (AH) into BL
	shl 	bl, 4           ; Shift BL left by 4 bits to make space for the background color
	and 	al, 0x0F        ; Ensure AL only contains the lower 4 bits (0-15) for the background color
	or 		bl, al          ; Combine BL (shifted text color) and AL (background color)

	; Store the combined color in bTextColor
	mov 	byte [bTextColor], bl

	; Restore the state of BX and AX registers
	pop 	bx
	pop 	ax
ret                       ; Return from the SetTextColor routine


; ********************************
; Variables
; ********************************

bTextColor	db	0x0F	; b - byte sized variable

%endif
