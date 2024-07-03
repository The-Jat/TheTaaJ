.PHONY: all clean

CC = gcc
ASM = nasm

ASMFLAGS = -f elf32
CFLAGS = -m32 -fno-pie -ffreestanding
LDFLAGS = -m elf_i386 -Ttext 0xB000 --oformat binary

BOOT_STAGE2_INCLUDE = ../boot/stage2/includes

KERNEL_INCLUDE = includes/

BUILD_DIR := build

# Find all c and asm source files
C_SOURCES := $(shell find . -name "*.c")
# $(wildcard *.c)
ASM_SOURCES := $(shell find . -name "*.asm")

# generate object files from source files
# C_OBJECTS := $(C_SOURCES:.c=.o)
C_OBJECTS := $(patsubst %.c, %.o, $(C_SOURCES))
ASM_OBJECTS := $(ASM_SOURCES:.asm=.o)

# All object files
OBJECTS := $(C_OBJECTS) $(ASM_OBJECTS)

all: Build kernel_entry_exec.elf

# kernel entry in binary
kernel_entry_exec.elf: kernel_entry.asm
	nasm -f elf -I $(BOOT_STAGE2_INCLUDE) $< -o build/kernel_entry_relocatable.elf
	ld -m elf_i386 -Ttext 0x1000000 -o build/kernel_entry_exec.elf build/kernel_entry_relocatable.elf

# kernel entry in elf
kernel_entry.elf: kernel_entry.asm
	nasm -f bin -I $(BOOT_STAGE2_INCLUDE) $< -o build/kernel_entry.elf

## Generates the ELF executable type file,
## Since the nasm -f elf32 option only generates Relocatable file.
## But our ELF loader is capable of parsing only Executable file
## for the time being.
kernel_entry_executable: kernel_entry.elf
	ld -m elf_i386 -T kernel.ld -o build/kernel.elf build/kernel_entry.elf

Build:
	@echo "building kernel build directory"
	mkdir -p build

kernel.bin: $(OBJECTS)
#	ld -m elf_i386 -Ttext 0xB000 --oformat binary -o build/$@ build/kernel_entry.o build/kernel_main.o build/idt.o build/idt_asm.o build/isr.o build/isr_asm.o build/vga.o build/kernel_utilities.o

# Rule to compile C files
%.o: %.c
	gcc  -m32 -fno-pie -ffreestanding -I $(KERNEL_INCLUDE) -c $< -o build/$(notdir $@)

%.o: %.asm
	nasm -f elf32 -I $(BOOT_STAGE2_INCLUDE) $< -o build/$(notdir $@)

# Clean up generated files
clean:
	rm -rf build
