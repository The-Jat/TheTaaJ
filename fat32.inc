; FAT32.inc - FAT32 helper functions and structures

; BIOS Parameter Block (BPB) offsets
BPB_BytsPerSec    equ 0x0B
BPB_SecPerClus    equ 0x0D
BPB_RsvdSecCnt    equ 0x0E
BPB_NumFATs       equ 0x10
BPB_FATSz32       equ 0x24
BPB_RootClus      equ 0x2C

; Directory entry offsets
DIR_Name          equ 0x00
DIR_Attr          equ 0x0B
DIR_NTRes         equ 0x0C
DIR_CrtTimeTenth  equ 0x0D
DIR_CrtTime       equ 0x0E
DIR_CrtDate       equ 0x10
DIR_LstAccDate    equ 0x12
DIR_FstClusHI     equ 0x14
DIR_WrtTime       equ 0x16
DIR_WrtDate       equ 0x18
DIR_FstClusLO     equ 0x1A
DIR_FileSize      equ 0x1C

FATStart          equ 0x7E00 + 512  ; Assuming FAT starts immediately after BPB

