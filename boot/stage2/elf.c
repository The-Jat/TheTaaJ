#include <elf.h>
#include <print.h>


// Define the memory start location where the ELF file is loaded
#define KERNEL_LOAD_START 0xb000
#define KERNEL_DEST_START 0x1000000


int load_elf32() {
	puts("load_elf32", VGA_COLOR_BLACK, VGA_COLOR_LIGHT_RED);

	Elf32_Header *header = (Elf32_Header *)KERNEL_LOAD_START;
	if (header->e_ident[0] != ELFMAG0 ||
	    header->e_ident[1] != ELFMAG1 ||
	    header->e_ident[2] != ELFMAG2 ||
	    header->e_ident[3] != ELFMAG3) {
		// ERROR not an ELF File
		puts("load_elf32, Not an ELF File", VGA_COLOR_BLACK, VGA_COLOR_LIGHT_RED);
		return 0;
	}
	
	//uintptr_t entry = (uintptr_t)header->e_entry;
	
	// Iterate through program headers
	for (int i = 0; i < header->e_phnum; i++) {
	// Calculate the address of the program header
	Elf32_Phdr *phdr = (Elf32_Phdr *)((unsigned char *)header + header->e_phoff + i * header->e_phentsize);
        
	// If the segment is loadable
	if (phdr->p_type == PT_LOAD) {
		// Calculate the source and destination addresses
		unsigned char *src = (unsigned char *)KERNEL_LOAD_START + phdr->p_offset;
		//unsigned char *dst = (unsigned char *)KERNEL_DEST_START + phdr->p_vaddr;
		unsigned char *dst = (unsigned char *)phdr->p_vaddr;

		// Copy the segment to the virtual address
		memcpy(dst, src, phdr->p_filesz);
            
		// Zero out the rest of the segment if p_memsz > p_filesz
		memset(dst + phdr->p_filesz, 0, phdr->p_memsz - phdr->p_filesz);
	}
    }
    
    return 1;
	
}
