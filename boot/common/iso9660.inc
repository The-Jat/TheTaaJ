%ifndef __ISO_9660_INC__
%define __ISO_9660_INC__

;;%define VOLUME_DESCRIPTOR_READ_LOCATION 0x0500 ; This is for the case when we need to
					; read the Volume descriptors in memory not in
					; buffer (fixed size buffer) in the code.

%define FILE_LOADING_AREA 0xCC00;0xB000	; It is the file loading area in memory map
%define ROOT_DIRECTORY_ENTRY_LOCATION 0x90000	; It is the subsystem memory area


; **************************
; ReadVolumeDescriptor:
; Reads the LBA 16 of the ISO disk and checks, if it is Primary Volume Descriptor.
; -- If it is, then Proceed further by extracting the Root Directory Entry and
;    printing the root directory entries.
; -- If not the PVD, then Read the next LBA which is 17, until we have the PVD or
;    we encounter the Volume set terminator descriptor (which marks the end of the descriptors)
;
; IN:
; 	- Nothing
; OUT:
;	- Nothing
; **************************
Read_volume_descriptors:
	pushad 		; save the state
	mov si, sReadingVolumeDescriptorsStatement
	call PrintString16BIOS
	call PrintNewline
.iterate_descriptor:
     xor eax, eax	; clear out the eax
     mov esi, eax	; clear out the esi
     mov ax, 0x0000
     mov es, ax		; set es to 0x0000
     mov bx, PrimaryVolumeDescriptor;VOLUME_DESCRIPTOR_READ_LOCATION
     
     mov eax, [dwVolumeDescriptorStartingSector]	; starting sector low 32 bit (0-indexed LBA)
     mov esi, 0		; starting sector high 32 bit
     
     mov ecx, 1		; number of sectors to read
     mov edx, 2048	; Sector sizes in bytes (1 sector = 2048 in ISO 9660)
     call ReadFromDiskUsingExtendedBIOSFunction

     ; Verify the PVD identifier 'CD001',
     ; which would be at offset 1 of Volume Descriptor structure.
     mov si, PrimaryVolumeDescriptor.PVD_StandardIdentifier;VOLUME_DESCRIPTOR_READ_LOCATION + 1
     mov cx, 5		; 5 characters to check
     mov di, iso_id
     repe cmpsb
     jne .invalid_iso_disk

    ;; We got the valid iso disk identifier.
    ; Print valid iso disk statement.
	push si
	mov si, sValidISODiskIdentifierString
	call PrintString16BIOS
	call PrintNewline
	pop si

    ;; Location where the descriptor was read, load it in al.
    
    ; Print the Volume Descriptor Type
	mov si, sVolumeDescriptorTypeStatement
	call PrintString16BIOS
	call PrintWordNumber
	call PrintNewline
 
    mov byte al, [PrimaryVolumeDescriptor.PVD_Type];[VOLUME_DESCRIPTOR_READ_LOCATION]
    ; Check if the descriptor is Primary Volume Descriptor.
    cmp al, 0x01	; Check for the Primary Volume Descriptor (PVD)
    jne .read_next_volume_descriptor	; If not PVD, read next descriptor
    
    ;; Got PVD
    ; Print the PVD statement
	mov si, sGotPVDStatement
	call PrintString16BIOS
	call PrintNewline

    ;; Read the Root Directory Entry from the PVD,
    ;; which is at offset 156.
    ;; Root Directory Entry is of 34 bytes.
;    mov si, VOLUME_DESCRIPTOR_READ_LOCATION
;    add si, 156		; Offset 156, where the Root Directory Entry Structure is located.
mov si, PrimaryVolumeDescriptor.PVD_DirectoryEntry
;    mov si, 0x809c	; si = 0x8000 + 156
    mov dword eax, es:[si + 2] ; Logical Block Address of the extent (first 4 bytes)
    xor edx, edx
    mov edx, eax		; Save LBA

    ;; Read the Root Directory Entry at Location 0x9000
    mov ax, SUBSYSTEM_MEM_SEGEMENT;0x9000;0x0000
    mov es, ax
    mov bx, SUBSYSTEM_MEM_OFFSET;0x00;ROOT_DIRECTORY_ENTRY_LOCATION ;0x9000
    
    mov eax, edx	; starting sector low 32 bit (0-indexed LBA)
    mov esi, 0		; starting sector high 32 bit
     
    mov ecx, 1		; number of sectors to read
    mov edx, 2048	; Sector sizes in bytes (1 sector = 2048 in ISO 9660)
    call ReadFromDiskUsingExtendedBIOSFunction

    ;; Read Root Directory Entry and Print its Entries Identifier.
    mov si, ROOT_DIRECTORY_ENTRY_LOCATION  ; 0x9000	; Memory Address where the Root Directory Entries (Record)
    			; is read.
;    call Find_and_Load_File_from_Root
    call Read_Root_Directory_Entry
   jmp .done_reading_volume_descriptor	; We are done reading and printing root directory entries.

;; Read the very next volume decriptor.
.read_next_volume_descriptor:
    cmp al, 0xFF			; Check for the end of volume descriptor list
    					; which is Volume Descriptor Set Terminator
    					; whose type (first byte) is 0xFF.
    je .volume_descriptor_terminator

    add dword [dwVolumeDescriptorStartingSector], 1	; read next sector i.e next volume descriptor.
    jmp .iterate_descriptor


;; Volume Descriptor Set Terminator
.volume_descriptor_terminator:
	mov si, volume_descriptor_terminator_encountered
	call PrintString16BIOS
	jmp .done_reading_volume_descriptor

.invalid_iso_disk:
	;; We got the Invalid ISO DISK
	; Print the invalid statement and go to infinite loop
	mov si, invalid_iso_disk_statement
	call PrintString16BIOS
	call PrintNewline

;;;TODO return the status, in case of failure as well as success.

.done_reading_volume_descriptor:
	popad		; restore the state
ret	; end of Read_volume_descriptors function.


; **************************
; Reads the Root Directory Entry which should be pointed
; by `SI`. It scans all the entries of the root directory
; and prints the identifier(name) of one and all.
; IN:
; 	- SI Root Directory Entry
; **************************
Read_Root_Directory_Entry:
	pushad		; save the state
.iterate_entry:
	xor ax, ax		; Clear out the ax register
	mov byte al, es:[si]	; it points to the 0 offset in directory entry.
				; At Offset 0 is length of the record.
				; It should be non zero, if zero means no valid record.
	test al, al
	
	jz .reading_done
	
	;; It is valid directory record.

	xor cx, cx
	xor dx, dx
	;mov cx, 10
	mov byte cl, es:[si + 32]	; get the file identifier length.
	mov byte dx, cx
	call PrintWordHex
	call PrintNewline
	mov di, si		; di and si both contains the current directory entry.
	add di, 33		; Add 33 to di which is the starting of
				; file identifier of the current directory entry.
.print_entries_identifier_char:
	mov byte al, es:[di]	; read the first character of current directory
				; entry's file identifier.
	call PrintChar16BIOS
	inc di			; jmp to next char in file identifier.
	loop .print_entries_identifier_char	; loop to print next char.
	call PrintNewline	; '\n'
	add byte si, es:[si]	; jump to next directory entry by adding
				; the length of the current directory entry.
	jmp .iterate_entry

.reading_done:
	popad 	; restore the state
ret


; **************************
; Reads the Root Directory Entry which should be pointed
; by `SI`. It scans all the entries of the root directory
; and prints the identifier(name) of one and all.
; IN:
; 	- SI Root Directory Entry
; LOCAL VARIABLE:
;	VAR1 = [bp - 4] = Address of the Current Directory entry.
;	VAR2 = [bp - 8] = Found file LBA.
;	VAR3 = [bp - 12] = Sector rounded off file size.
; **************************
Find_and_Load_File_from_Root:
	pusha		; save the state
	push bp		; Save old base pointer
	mov bp, sp	; Set new base pointer
	sub sp, 12	; Allocate 8 bytes for local variables
			; 2 local variables of size 4 bytes each
	mov word [bp - 4], si	; store the address of first entry
				; of Root Directory Entries.
mov ax, SUBSYSTEM_MEM_SEGEMENT
mov es, ax
	; Print the function job message
	; SI - is needy for the function so push the initial state into the stack
	; and pop after printing the string. Thus storing the initial value.
	push si
	mov si, sFindAndLoadFileStatement
	call PrintString16BIOS
	call PrintNewline
	pop si

.iterate_entry:

	xor ax, ax		; Clear out the ax register
	mov byte al, es:[si]	; it points to the 0 offset in directory entry.
				; At Offset 0 is length of the record.
				; It should be non zero, if zero means no valid record.
	test al, al
	
	jz .didnt_find		; Reached the end of the directory entry list
				; and didnt find the file

	;; It is valid directory record.

	; Get the File Flags field (offset 25 from the start of the entry).
	mov byte al, es:[si + 25]
	; Check if it's a directory
	test al, 0x02	; Check if bit 1 is set
	jnz .is_directory

	;; It is a file
	
	;; Check it's identifier.
	xor cx, cx	; clear out cx
	xor dx, dx	; clear out dx
	mov byte cl, es:[si + 32]	; get the file identifier length,
					; which is at offset 32 of directory_entry_record.
	mov byte dx, cx			; Store the identifier length
					; into the DX.

	;; Check the file identifier
	mov di, si	; DI and SI points to the Directory Entry Record
	add di, 33	; add offset 33, which is the start of the file identifier
			; DI, now points to the file identifier beginning.
	mov cx, 6	; The length of the file identifier.
	lea si, [file_identifier]	; Load SI to the identifier of the required file.
	repe cmpsb	; Keep checking the SI and DI for the match.
			; if the length of the file identifier in both DI and SI matches,
			; then we have the required file, else jump to the next
			; directory entry record.
	jne .onto_next	; Jump to the next directory record if file identifier of
			; particular size mismatch

	;; Found the file

	mov si, [bp-4]	; Store the current Directory Entry Record in the Local variable.

.got_file:
	;; got the file
	; Print the message claiming that we found the file, you searched for.
	push si
	mov si, sFoundTheFile
	call PrintString16BIOS
	call PrintNewline
	pop si

	;; Location of the extent of the File
	; i.e the LBA of the data of the file.
	mov eax, es:[si + 2] ; Logical Block Address of the extent (first 4 bytes)

	;; Save LBA, in local variable
	;; Store File LBA in local variable BP - 8
	mov dword [bp - 8], eax		; Store the File LBA
					; to second local variable.

	; Read the file at the FILE_LOADING AREA which is
	; 0x0500
	
	; Save File Data Length in local variable
	xor eax, eax
	mov byte ax, es:[si+ 10]	; Data length of the file in LSB
				; It is present at offset 10 of Directory Entry Record.
				; It is of 4 byte in size
				; and data length is in bytes.
	mov dword [bp - 12], eax

	;; Prints the Data length
	 push si
	 mov si, sLengthOfFile
	 call PrintString16BIOS
	 call PrintWordNumber
	 call PrintNewline
	 pop si

	mov ax, 0x0000
	mov es, ax		; Zero the extra segment.
	mov bx, FILE_LOADING_AREA ; 0xB000

	;; Roundoff the size to the 2048 which is the sector size.
	; i.e if the data length is less than 2048, then sector to load should be
	; 1 which is 2048 bytes
	; If the data length is more than 2048, 2049 for instance, one more than 2048
	; then the sector to load will be 2 = (2048*2)bytes.
	; Now eax, consists the rounded off value to sector size
	mov eax, [bp - 12]	; Actual data length of the file.
	add eax, 2047
	mov ecx, 2048
	div ecx		; division also affect the edx register.
			; so be careful, if you have stored the necessary
			; information in edx, it must have got wiped out.
	; move the sector count to ecx.
	mov ecx, eax

	mov eax, [bp - 8]	; starting sector low 32 bit (0-indexed LBA)
				; fetch from the second local variable.
	mov esi, 0	; starting sector high 32 bit
	;mov ecx, 1	; Here you can explicitly specify the sector count,
			; Otherwise we already calculated the rounded off
			; sector count to sector size based on the data length.
	mov edx, 2048	; Sector sizes in bytes (1 sector = 2048 in ISO 9660)
	call ReadFromDiskUsingExtendedBIOSFunction

;jmp FILE_LOADING_AREA

	;; Tell user about displaying the content of the file.
	mov si, sPrintingTheFileContent
	call PrintString16BIOS
	call PrintNewline

	;; Print the contents of the file.
	mov si, FILE_LOADING_AREA	; 0xB000
					; Prints the data from the 
					; file loaded area.
	call PrintString16BIOS
	call PrintNewline

	; We are done here reading the contents of the file.
	jmp .reading_done

.is_directory:
	; handle the directory, for the time being, just jump to next entry.
	; Maybe you can recursively search for the file in every directory.
	; Or you can search for particular directory.

;; Update the si to point to next entry.
.onto_next:
	; load the base address of current entry back to si
	mov word si, [bp - 4]

	add byte si, es:[si]	; increment si to point to next entry
				; Increment by adding the size of current entry.
				; at offset 0 of directory entry, we have the size
				; of each directory entry. we can get to the second
				; entry by adding the size of current entry to the
				; current entry address.
	mov word [bp - 4], si	; update the next entry address in the local
				; variable as well.
	jmp .iterate_entry	; onto the next entry.

;; Sorry but we didnt find your file.
.didnt_find:
	mov si, sDidntFindTheFile
	call PrintString16BIOS
	call PrintNewline

.reading_done:
	add sp, 8	; Clear local variables
	mov sp, bp	; Reset Stack pointer
	pop bp		; Restore old base pointer
	popa 	; restore the state
;; End of the Find_and_Load_File_from_Root function
ret



;; Data for the Find_and_Load_File_from_Root function
sLengthOfFileIdentifierStatement: db 'Length of the File Identifier is (in bytes): ', 0
sLengthOfFile: db 'Length of the File (in bytes): ', 0
sUnpaddedSizeStatement: db 'Un-Padded Size of Bootloader (bytes): ', 0
sFindAndLoadFileStatement: db 'Find and Load the file.', 0
sFoundTheFile: db 'Found the file.', 0
sPrintingTheFileContent: db 'Printing the content of the file.', 0
sDidntFindTheFile: db 'Didnt find the file.', 0
file_identifier: db 'AB.TXT', 0	;'AB.TXT', 0



dwVolumeDescriptorStartingSector dd 16	;; starting sector where the volume descriptor resides.

invalid_iso_disk_statement: db 'Invalid ISO disk identifier.', 0
volume_descriptor_terminator_encountered: db 'Volume Descriptor Set Terminator Encountered.', 0
sReadingVolumeDescriptorsStatement: db 'Reading Volume Descriptors...', 0
sValidISODiskIdentifierString: db 'It`s valid ISO Disk Identifier.', 0
sVolumeDescriptorTypeStatement: db 'Volume Descriptor Type = ', 0
sGotPVDStatement: db 'We got the PVD.', 0

;sItsDirectoryStatement: db 'Its a directory.', 0
iso_id db 'CD001'
%endif
