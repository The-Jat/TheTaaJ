%ifndef _STAGE_2_MEMORY_INC_
%define _STAGE_2_MEMORY_INC_

BITS 16

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;*********************************
; GetLowerMemorySize
; IN: Nothing
; OUT: Nothing
GetLowerMemorySize:
	pusha
	clc		; Clear the carry flag
	int 0x12	; Interrupt to get the number of low (conventional) memory
	jc .GetLowerMemory_Error
	
	; if carry flag is not set means function successfully returned
	; size of lower memory in AX register
	; AX is having the count in KB
	
	mov si, sLowerMemoryCountSentence	; display lower memory count sentence.
	call PrintString16BIOS
	
	;; Now print the size from the AX in decimal
	call PrintWordNumber		; This ones display the contents of AX in decimal
	call PrintNewline		; \n
	
	popa		; pop all saved registers.

ret	; return to the caller	

; Trap code
.GetLowerMemory_Error:
	mov si, sInt10hFailed		; Error string
	call PrintString16BIOS
	call PrintNewline		; \n
hlt
; ********************************
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Variables

sLowerMemoryCountSentence db 'Lower Memory Count : ', 0
sInt10hFailed		db	'Int 0x10 Failed to return lower memory size', 0

%endif
