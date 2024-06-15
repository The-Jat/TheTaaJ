%ifndef _STAGE_2_MEMORY_INC_
%define _STAGE_2_MEMORY_INC_

BITS 16

%define 		BIOS_MEMMAP_SIGNATURE		0x0534D4150

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
; ********************************
; LoadMemoryMap
; IN:
;	- ES:EDI: Destination Buffer
; OUT:
; 	- EBX: 0 -> Success, 1 -> Err
; ********************************
LoadMemoryMap:
	; Save buffer
	push 	edi

	; Clear out space for memory map (1 Page)
	xor 	eax, eax	; Zero out EAX, common and efficient way to clear a register
	mov 	ecx, 1024	; counter
	rep 	stosd		; `stosd` =  string instruction that uses `EDI` register as a
				; destination pointer. It writes the value of `EAX`
				; to the memory location pointed to by `EDI`
				; `rep` = repeat the `stosd` instruction `ECX` times (1024)
				; times
				; The whole block code zeroes out 1024 double words (each 4
				; bytes) of memory start from the address to by `EDI`
				; It effectively fills a 4096 bytes (1024*4) block of memory
				; with zeroes.

	; Restore
	pop 	edi

	; Setup INT 0x15
	mov eax, 0xE820		; Function number  = E820
	xor ebx, ebx		; Zero out ebx
	xor ebp, ebp		; Zero out base pointer
	mov ecx, 24		; Size of the structure (including ACPI attributes)
	mov edx, BIOS_MEMMAP_SIGNATURE	; SMAP signature

	; Ready, make sure we force a valid ACPI 3.X Entry
	mov [es:di + 20], dword 1

	int 0x15	; Read Memory Map
	jc .NoMemoryMap	; unsuccessful call if carry flag is set.

	; Restore EDX for safety (EAX contains signature on success)
	mov 	edx, BIOS_MEMMAP_SIGNATURE
	cmp 	eax, edx		; Compare the signature
	jne 	.NoMemoryMap		; if mismatch unsuccessful

	; If ebx is 0, only 1 entry, we cant use that for anything
	test 	ebx, ebx	; Performs bitwise AND operation between EBX
				; and then set the processor's flag based on the result.
				; This is common idiom in assembly language to check
				; is a register is zero or not.
	je 	.NoMemoryMap	; if zero, means no memory map
	
	; Proceed if memory map is valid
	jmp 	.MediasRes

	; Each call returns one memory map entry at a time.
	; Loop for subsequent int 0x15 calls
	.cLoop:
		; Query time
		mov 	eax, 0xE820	; Function number for memory map query
		mov 	ecx, 24		; Size of memory map structure
		mov 	[es:di + 20], dword 1	; Set ACPI attribute to 1
		int 	0x15			; Invoke BIOS interrupt 0x15
		jc 		.Done		; Jump to Done if carry flag is set (end of memory map)
		mov 	edx, BIOS_MEMMAP_SIGNATURE

	.MediasRes:
		; Parse Entry
		test 	ecx, ecx		; Test if ecx is zero
		je 		.SkipEntry	; Jump to SkipEntry if ecx is zero
		cmp 	cl, 20			; Compare lower byte of ecx with 20
		jbe 	.Acpi2			; Jump to Acpi2 if cl <= 20
		test 	byte [es:di + 20], 1	; Test if ACPI attribute bit 0 is set
		je 		.SkipEntry	; Jump to Skip Entry if ACPI attribute bit 0 is set

	; ACPI 2.0 Entry
	.Acpi2:
		; Acpi2 Entry

		; Get lower dword of memory length
		mov 	ecx, [es:di + 8]
		; Check if lowe dword is not zero
		test 	ecx, ecx	; Test if ecx is zero
		jne 	.GoodEntry	; Jump to GoodEntry if ecx is not zero

		; Get higher dword of memory length
		mov 	ecx, [es:di + 12]	; Get higher dword of memory length
		test 	ecx, ecx		; Test if ecx is zero
		je 		.SkipEntry

	.GoodEntry:
		; Increase entry count & move to next
		inc 	ebp		; Increment ebp (entry count)
		add 	edi, 0x20	; Move edi to the next memory map entry (each entry is 32 bytes)

	.SkipEntry:
		; If ebx resets to 0, list is complete
		test 	ebx, ebx		; Test if ebx is zero
		jne 		.cLoop		; Jump to cLoop if ebx is not zero
		jmp 		.Done		; Jump to done if ebx is zero

	.NoMemoryMap:
		mov	si, sNoMemoryMapSentence
		call	PrintString16BIOS
		call	PrintNewline
		mov 	ebx, 1			; set ebx to 1  (indicating failure)
		mov 	ebp, 0			; Clear ebp (entry count)
		ret				; Return

	.Done:
		mov 	ebx, 0			; Set ebx to 0 (indicating success)
ret



; ********************************
; SetupMemory
; ********************************
SetupMemory:
	; Save state 
	pusha


	; Print
	mov si, sReadingMemoryMapSentence
	call PrintString16BIOS
	call PrintNewline


	; Step 1, try to get memory map
	mov 	edi, MEMLOCATION_MEMORY_MAP
	mov 	dword [dataStructureObj + DataStructure.MemMapAddr], edi
	mov 	si, sMemMapAddrSentence
	call	PrintString16BIOS

	mov	word dx, [dataStructureObj + DataStructure.MemMapAddr]
	call	PrintWordHex
	call	PrintNewline

	;; Memory Map function
	call 	LoadMemoryMap

	; Save entry count
	mov 	dword [dataStructureObj + DataStructure.MemMapLength], ebp

	;; Print the 
	mov	word ax, [dataStructureObj + DataStructure.MemMapLength]	
	mov	si, sMemMapLengthSentence
	call	PrintString16BIOS
	call	PrintWordNumber
	call	PrintNewline

	; Restore
	popa
ret
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Variables



sLowerMemoryCountSentence db 'Lower Memory Size (In KB) : ', 0
sInt10hFailed		db	'Int 0x10 Failed to return lower memory size', 0

sHigherMemoryCountSentence db 'Higher Memory Size (In KB) : ', 0
sInt15hFailed		db	'Int 0x15 Failed to return higher memory size', 0


sReadingMemoryMapSentence db 'Reading Memory Map...', 0
sNoMemoryMapSentence db 'No Memory Map...', 0

sMemMapAddrSentence db 'Memory Map Address is: ', 0
sMemMapLengthSentence db 'Memory Map Length is: ', 0

%endif
