%ifndef _STAGE_2_DATA_STRUCTURE_INC_
%define _STAGE_2_DATA_STRUCTURE_INC_

BITS 16

;; Declaration of structure
struc DataStructure
	.MemMapAddr	resd	1
	.MemMapLength	resd	1
	
endstruc

;; Initialization of structure
dataStructureObj:
istruc DataStructure
	at DataStructure.MemMapAddr,	dd 0
	at DataStructure.MemMapLength,	dd 0
iend


%endif
