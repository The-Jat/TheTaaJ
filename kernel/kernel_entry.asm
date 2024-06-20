; org 0xb000               ; Set the origin address for the code. This tells the assembler
                         ; that the code should be loaded at memory address 0x0B00.
                         ; So all the jmp statement and string declaration offset is
                         ; calculated based on it.
;; In ELF file format org directive is invalid.
;; org directive is only for the binary output format.                     


BITS 32                  ; Specify that the code is 32-bit.

kernel_entry:            ; Label for the kernel entry point.

jmp start		; jmp after the includes

;; Include files
%include "print32.inc"
extern k_main
start:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;; Clear the Screen
	call ClearScreen32
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;; Print Welcome Message
	mov esi, sKernelWelcomeStatement
	mov bl, YELLOW		; Foreground color = Yellow
	mov bh, BLACK		; Background color = Black
	call PrintString32
	;;
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;; Call the C kernel
	call k_main
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	

jmp $		; Infinite loop to halt execution after printing the message.


;; Put data in second sector
times 512 - ($ - $$) db 0

sKernelWelcomeStatement: db 'Welcome to Flat Binary 32-Bit Kernel Land.', 0
                         ; Define the welcome message string, terminated by a null byte (0).

times 1024 - ($ - $$) db 0	; Fill the rest of the 1 KB (1024 bytes) space with zeros.
