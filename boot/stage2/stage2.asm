BITS 16

%include "boot/common/defines.inc" ; contains defintion for STAGE_2_LOAD_ADDRESS

ORG STAGE_2_LOAD_ADDRESS	; 0x0500

; Memory Map:
; 0x00000000 - 0x000004FF		Reserved
; 0x00000500 - 0x00007AFF		Second Stage Bootloader (~29 Kb)
; 0x00007B00 - 0x00007BFF		Stack Space (256 Bytes)
; 0x00007C00 - 0x00007DFF		Bootloader (512 Bytes)
; 0x00007E00 - 0x00008FFF		Used by subsystems in this bootloader
; 0x00009000 - 0x00009FFF		Memory Map
; 0x0000A000 - 0x0000AFFF		Vesa Mode Map / Controller Information
; 0x0000B000 - 0x0007FFFF		File Loading Bay

jmp stage2_entry

; Includes
%include "boot/common/print16.inc"

stage2_entry:
	mov si, WelcomeToStage2		; Print Stage 2 Welcome message
	call PrintString16BIOS
	call PrintNewline		; \n
	call PrintWordNumber		; Print the Received AX value

jmp $

; **********
; Variables
; **********

WelcomeToStage2 db 'Welcome to the Stage2', 0
times STAGE_2_SIZE - ($-$$) db 0		; Fill up the remaining space with zeroes
