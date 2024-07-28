#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define EI_NIDENT 16

typedef struct {
    unsigned char e_ident[EI_NIDENT];
    uint16_t e_type;
    uint16_t e_machine;
    uint32_t e_version;
    uint32_t e_entry;
    uint32_t e_phoff;
    uint32_t e_shoff;
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsize;
    uint16_t e_phnum;
    uint16_t e_shentsize;
    uint16_t e_shnum;
    uint16_t e_shstrndx;
} __attribute__((packed)) Elf32_Ehdr;

typedef struct {
    uint32_t p_type;
    uint32_t p_offset;
    uint32_t p_vaddr;
    uint32_t p_paddr;
    uint32_t p_filesz;
    uint32_t p_memsz;
    uint32_t p_flags;
    uint32_t p_align;
} __attribute__((packed)) Elf32_Phdr;

typedef struct {
    uint32_t sh_name;
    uint32_t sh_type;
    uint32_t sh_flags;
    uint32_t sh_addr;
    uint32_t sh_offset;
    uint32_t sh_size;
    uint32_t sh_link;
    uint32_t sh_info;
    uint32_t sh_addralign;
    uint32_t sh_entsize;
} __attribute__((packed)) Elf32_Shdr;

void print_elf_header(const Elf32_Ehdr *hdr) {
    printf("ELF Header:\n");
    printf("  Type:                              %u\n", hdr->e_type);
    printf("  Machine:                           %u\n", hdr->e_machine);
    printf("  Version:                           %u\n", hdr->e_version);
    printf("  Entry point address:               0x%x\n", hdr->e_entry);
    printf("  Start of program headers:          %u (bytes into file)\n", hdr->e_phoff);
    printf("  Start of section headers:          %u (bytes into file)\n", hdr->e_shoff);
    printf("  Flags:                             0x%x\n", hdr->e_flags);
    printf("  Size of this header:               %u (bytes)\n", hdr->e_ehsize);
    printf("  Size of program headers:           %u (bytes)\n", hdr->e_phentsize);
    printf("  Number of program headers:         %u\n", hdr->e_phnum);
    printf("  Size of section headers:           %u (bytes)\n", hdr->e_shentsize);
    printf("  Number of section headers:         %u\n", hdr->e_shnum);
    printf("  Section header string table index: %u\n", hdr->e_shstrndx);
}

void parse_program_header(FILE *file, const Elf32_Ehdr *hdr) {
    Elf32_Phdr phdr;
    fseek(file, hdr->e_phoff, SEEK_SET);

    printf("\nProgram Header Table:\n");

    for (int i = 0; i < hdr->e_phnum; i++) {
        fread(&phdr, sizeof(phdr), 1, file);
        printf("  Program Header %d:\n", i);
        printf("    Type:         %u\n", phdr.p_type);
        printf("    Offset:       0x%x\n", phdr.p_offset);
        printf("    Virtual Addr: 0x%x\n", phdr.p_vaddr);
        printf("    Physical Addr:0x%x\n", phdr.p_paddr);
        printf("    File Size:    %u\n", phdr.p_filesz);
        printf("    Mem Size:     %u\n", phdr.p_memsz);
        printf("    Flags:        0x%x\n", phdr.p_flags);
        printf("    Align:        %u\n", phdr.p_align);
    }
}

void parse_section_header(FILE *file, const Elf32_Ehdr *hdr) {
    Elf32_Shdr shdr;
    fseek(file, hdr->e_shoff, SEEK_SET);

    // Read the section header string table first
    fseek(file, hdr->e_shoff + hdr->e_shstrndx * hdr->e_shentsize, SEEK_SET);
    fread(&shdr, sizeof(shdr), 1, file);

    char *strtab = malloc(shdr.sh_size);
    fseek(file, shdr.sh_offset, SEEK_SET);
    fread(strtab, shdr.sh_size, 1, file);

    printf("\nSection Header Table:\n");

    for (int i = 0; i < hdr->e_shnum; i++) {
        fseek(file, hdr->e_shoff + i * hdr->e_shentsize, SEEK_SET);
        fread(&shdr, sizeof(shdr), 1, file);
        
        printf("  Section Header %d:\n", i);
        printf("    Name:         %s\n", &strtab[shdr.sh_name]);
        printf("    Type:         %u\n", shdr.sh_type);
        printf("    Flags:        0x%x\n", shdr.sh_flags);
        printf("    Address:      0x%x\n", shdr.sh_addr);
        printf("    Offset:       0x%x\n", shdr.sh_offset);
        printf("    Size:         %u\n", shdr.sh_size);
        printf("    Link:         %u\n", shdr.sh_link);
        printf("    Info:         %u\n", shdr.sh_info);
        printf("    Address Align:%u\n", shdr.sh_addralign);
        printf("    Entry Size:   %u\n", shdr.sh_entsize);
    }

    free(strtab);
}

int main(int argc, char *argv[]) {
    // Ensure the user has provided an ELF file path.
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <ELF file path>\n", argv[0]);
        return 1;
    }

    // Open the provided ELF file.
    FILE *file = fopen(argv[1], "rb");
    if (!file) {
        perror("Failed to open file");
        return 1;
    }

    Elf32_Ehdr hdr;
    fread(&hdr, sizeof(hdr), 1, file);

    print_elf_header(&hdr);
    parse_program_header(file, &hdr);
    parse_section_header(file, &hdr);

    fclose(file);

    return 0;
}

