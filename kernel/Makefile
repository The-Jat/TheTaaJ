
.PHONY: all clean

CC = gcc
ASM = nasm

ASMFLAGS = -f elf32
CFLAGS = -m32 -fno-pie -ffreestanding
LDFLAGS = -m elf_i386 -Ttext 0xB000 --oformat binary

BOOT_STAGE2_INCLUDE = ../boot/stage2/includes

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

all: Build kernel.bin

Build:
	@echo "building build directory"
	mkdir -p build

kernel.bin: $(OBJECTS)
	ld -m elf_i386 -Ttext 0xB000 --oformat binary -o build/$@ build/kernel_entry.o build/kernel_main.o build/idt.o build/idt_asm.o build/isr.o build/isr_asm.o 

# Rule to compile C files
%.o: %.c
	@echo $<
	@echo $@
	gcc  -m32 -fno-pie -ffreestanding -c $< -o build/$(notdir $@)

%.o: %.asm
	nasm -f elf32 -I $(BOOT_STAGE2_INCLUDE) $< -o build/$(notdir $@)

# Clean up generated files
clean:
	rm -rf build
