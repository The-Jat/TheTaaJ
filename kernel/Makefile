# Kernel Root Makefile

# Define ROOT_DIR before including config.mk
ROOT_DIR := $(abspath .)

# Include the configuration file
include $(ROOT_DIR)/config.mk

# Define Build directory
BUILD_DIR := build

# Define directories
SUBDIRS = idt utilities vga

# Define source files in the root directory
ROOT_ASM_SOURCES = kernel_entry.asm
ROOT_C_SOURCES = kernel_main.c

# Define object files in the root directory
ROOT_ASM_OBJECTS = $(ROOT_ASM_SOURCES:%.asm=$(BUILD_DIR)/%.o)
ROOT_C_OBJECTS = $(ROOT_C_SOURCES:%.c=$(BUILD_DIR)/%.o)
ROOT_OBJECTS = $(ROOT_ASM_OBJECTS) $(ROOT_C_OBJECTS)

# generate object files from source files
C_OBJECTS := $(patsubst %.c, %.o, $(C_SOURCES))
ASM_OBJECTS := $(ASM_SOURCES:.asm=.o)

# All object files
OBJECTS := $(C_OBJECTS) $(ASM_OBJECTS)

# Default target
all: $(BUILD_DIR) kernel


## Generates the ELF executable type file,
## Since the nasm -f elf32 option only generates Relocatable file.
## But our ELF loader is capable of parsing only Executable file
## for the time being.
#kernel_entry_executable: kernel_entry.elf
#	ld -m elf_i386 -T kernel.ld -o build/kernel.elf build/kernel_entry.elf


# Create build directory if not exists
$(BUILD_DIR):
	@echo "Building kernel build directory"
	mkdir -p $(BUILD_DIR)

# Recursively call make in each subdirectory
$(SUBDIRS):
	$(MAKE) -C $@ ROOT_DIR=$(ROOT_DIR)

# Compile root C sources
$(BUILD_DIR)/%.o: %.c
	@echo "Compiling $<..."
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDES) -I $(KERNEL_INCLUDE)

# Assemble root ASM sources
$(BUILD_DIR)/%.o: %.asm
	@echo "Assembling $<..."
	$(AS) -f elf -I $(BOOT_STAGE2_INCLUDE) $< -o $@

# Build kernel
kernel: $(SUBDIRS) $(ROOT_OBJECTS)
	@echo "Linking kernel..."
	ld $(LDFLAGS) -T kernel.ld -o $(BUILD_DIR)/kernel.elf $(BUILD_DIR)/kernel_entry.o $(filter-out $(BUILD_DIR)/kernel_entry.o, $(wildcard $(BUILD_DIR)/*.o))
#$(wildcard $(BUILD_DIR)/*.o)

#kernel.bin: $(OBJECTS)
#	ld -m elf_i386 -Ttext 0xB000 --oformat binary -o build/$@ build/kernel_entry.o build/kernel_main.o build/idt.o build/idt_asm.o build/isr.o build/isr_asm.o build/vga.o build/kernel_utilities.o

# Rule to compile C files
#%.o: %.c
#	@echo $@
#	gcc  -m32 -fno-pie -ffreestanding -I $(KERNEL_INCLUDE) -c $< -o build/$(notdir $@)

#%.o: %.asm
#	nasm -f elf32 -I $(BOOT_STAGE2_INCLUDE) $< -o build/$(notdir $@)

# Clean up generated files
clean:
	rm -rf $(BUILD_DIR)
	for dir in $(SUBDIRS); do $(MAKE) -C $$dir ROOT_DIR=$(ROOT_DIR) clean; done
#	for dir in $(SUBDIRS); do $(MAKE) -C $$dir clean; done

# Phony targets
.PHONY: all clean $(SUBDIRS) kernel
