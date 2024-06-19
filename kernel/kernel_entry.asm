org 0xb000               ; Set the origin address for the code. This tells the assembler
                         ; that the code should be loaded at memory address 0x0B00.
                         
BITS 16                  ; Specify that the code is 16-bit.

kernel_entry:            ; Label for the kernel entry point.

mov si, sKernelWelcomeStatement
                         ; Load the address of the welcome message string into SI register.
                         
printString:
	lodsb                    ; Load the byte at [SI] into AL and increment SI.
	test al, al              ; Test if AL is zero.
	je .printDone            ; If AL is zero, jump to .printDone label (end of string).
	mov ah, 0x0e             ; Set AH to 0x0E, the BIOS teletype function for displaying characters.
	int 0x10                 ; Call BIOS interrupt 0x10 to display the character in AL.
	jmp printString          ; Repeat the loop to print the next character.

.printDone:
jmp $                    ; Infinite loop to halt execution after printing the message.


;; Put data in second sector
times 512 - ($ - $$) db 0

sKernelWelcomeStatement: db 'Welcome to Flat Binary Kernel Land.', 0
                         ; Define the welcome message string, terminated by a null byte (0).

times 1024 - ($ - $$) db 0	; Fill the rest of the 1 KB (1024 bytes) space with zeros.
