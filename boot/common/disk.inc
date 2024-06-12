%ifndef _COMMON_DISK_INC_
%define _COMMON_DISK_INC_

; ********************************
; ReadFromDisk
; IN:
; 	- DL: Drive number (0x00 = floppy A:, 0x80 = first HDD, etc.)
;	- CH: Cylinder number
;	- DH: Head number
;	- CL: Sector number (1-63)
;	- ES:BX: Memory address to load data to
;	- AL: Number of sectors to read
; ********************************
ReadFromDisk:
	; Save state
	pushad

	; Prepare for BIOS disk read
	mov 	ah, 0x02          ; BIOS function: Read sectors
	int 	0x13              ; Call BIOS disk interrupt

	; Check for errors
	jc 	.DiskReadError       ; If carry flag is set, jump to error handler

	; Restore state & return
	popad
	ret

.DiskReadError:
	; Handle disk read error (could be retry logic or error message)
	mov si, diskErrorMessage
	call PrintString16BIOS
	hlt
	jmp 	.DiskReadError
	
diskErrorMessage db 'Disk read error!', 0
%endif
