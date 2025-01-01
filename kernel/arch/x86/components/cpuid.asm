BITS 32

global CpuId
global CpuEnableFpu
global CpuEnableSse

; Assembly routine to enable
; sse support
CpuEnableSse:
	; Save EAX
	push eax

	; Enable
	mov eax, cr4
	bts eax, 9		;Set Operating System Support for FXSAVE and FXSTOR instructions (Bit 9)
	bts eax, 10		;Set Operating System Support for Unmasked SIMD Floating-Point Exceptions (Bit 10)
	mov cr4, eax

	; Restore EAX
	pop eax
	ret 

; Assembly routine to enable
; fpu support
CpuEnableFpu:
	; Save EAX
	push eax

	; Enable
	mov eax, cr0
	bts eax, 1		;Set Monitor co-processor (Bit 1)
	btr eax, 2		;Clear Emulation (Bit 2)
	bts eax, 5		;Set Native Exception (Bit 5)
	btr eax, 3		;Clear TS
	mov cr0, eax

	finit

	; Restore
	pop eax
	ret 


; Assembly routine to get
; cpuid information
; void cpuid(uint32_t cpuid, uint32_t *eax, uint32_t *ebx, uint32_t *ecx, uint32_t *edx)
CpuId:
	; Stack Frame
	push ebp
	mov ebp, esp
	pushad

	; Get CPUID
	mov eax, [ebp + 8]
	cpuid
	mov edi, [ebp + 12]
	mov [edi], eax
	mov edi, [ebp + 16]
	mov [edi], ebx
	mov edi, [ebp + 20]
	mov [edi], ecx
	mov edi, [ebp + 24]
	mov [edi], edx

	; Release stack frame
	popad
	pop ebp
ret 
