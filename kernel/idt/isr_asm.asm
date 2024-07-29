; Interrupt Service Routines
global _isr0

;  0: Divide By Zero Exception
_isr0:
	cli
	push byte 0
	push byte 0	; Interrupt number
	jmp isr_common_stub


extern fault_handler

isr_common_stub:
	pusha
	push ds
	push es
	push fs
	push gs
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov eax, esp
	push eax
	mov eax, fault_handler
	call eax
	pop eax
	pop gs
	pop fs
	pop es
	pop ds
	popa
	add esp, 8
	iret
