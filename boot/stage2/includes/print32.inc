%ifndef _PRINT_32_INC_
%define _PRINT_32_INC_

BITS 32

%define VGA_DEFAULT_MEM_ADDR	0xb8000

%define VGA_ROW		80
%define VGA_COLOUMN	25

; 80*25 Colors
%define 		BLACK		0x0
%define 		BLUE 		0x1
%define 		GREEN 		0x2
%define 		CYAN 		0x3
%define 		RED 		0x4
%define 		MAGENTA 	0x5
%define 		BROWN 		0x6
%define 		LGRAY 		0x7
%define 		DGRAY		0x8
%define 		LBLUE		0x9
%define 		LGREEN		0xA
%define 		LCYAN		0xB
%define 		LRED		0xC
%define 		LMAGENTA	0xD
%define 		YELLOW		0xE
%define 		WHITE		0xF


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; **************************
; ClearScreen32
; IN:
	; BL: Foreground color
	; BH; Background color
;	Nothing
; Registers:
; 	- Conserves all the Registers
; **************************
ClearScreen32:
	pushad		; Push the complete state
	
	mov esi, VGA_DEFAULT_MEM_ADDR
	mov ecx, VGA_ROW * VGA_COLOUMN
.ClearScreen32Loop:
	mov byte [esi], ' '		; Space
	mov byte [esi + 1], 0x07	; default color - lightgrey on black
	
	add esi, 2			; Increment VGA memory pointer by 2 bytes.

	loop .ClearScreen32Loop		; Loop until ecx is 0
	popad		; Pop all the saved state

ret
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; **************************
; PrintString32
; IN:
; 	- ESI: Null-Terminated String
; Registers:
; 	- Conserves all the registers
; **************************
PrintString32:
	pushad		; push all 32 bit registers onto the stack
	
	mov ecx, VGA_ROW * VGA_COLOUMN
	mov edi, VGA_DEFAULT_MEM_ADDR
	
	shl bh, 4
	or bl, bh
	
.PrintString32Loop:
	lodsb		; it reads character from the esi to al
	test al, al
	je .PrintString32Done

	;; The first byte in 80*25 ie. mode 0x3 has to be the character
	;; followed by the color,
	;; 	which is lower 4 bits for foreground color
	;; 	upper 4 bits for background color
	mov [edi], al			; Write the character
	mov byte [edi + 1], bl;0x07	; color
	
	add edi, 2
	
	loop .PrintString32Loop
.PrintString32Done:
	
	popad		; pop all saved 32-bit register from the stack
ret
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

%endif
