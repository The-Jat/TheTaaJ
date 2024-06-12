BITS 16

%include "boot/common/defines.inc" ; contains defintion for STAGE_2_LOAD_ADDRESS

ORG STAGE_2_LOAD_ADDRESS	; 0x0500

jmp stage2_entry

; Includes
%include "boot/common/print16.inc"

stage2_entry:
cli
	mov si, WelcomeToStage2
	call PrintString16BIOS

jmp $

; **********
; Variables
; **********

WelcomeToStage2 db 'Welcome to the Stage2', 0
times STAGE_2_SIZE - ($-$$) db 0		; Fill up the remaining space with zeroes
