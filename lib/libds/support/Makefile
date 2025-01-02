# lib/libds/support/Makefile

# Ensure ROOT_DIR is set, defaulting to one level up if not provided
ROOT_DIR ?= $(abspath ..)

# Include the configuration file
include $(ROOT_DIR)/config.mk

# Define directories
BUILD_DIR = $(ROOT_DIR)/build
CURRENT_DIR = .

# Define source files
ASM_SOURCES =
C_SOURCES = ds.c

# Define object files
ASM_OBJECTS = $(ASM_SOURCES:%.asm=$(BUILD_DIR)/%.o)
C_OBJECTS = $(C_SOURCES:%.c=$(BUILD_DIR)/%.o)
OBJECTS = $(ASM_OBJECTS) $(C_OBJECTS)

# Build all
all: $(BUILD_DIR) $(OBJECTS)

# Compile C sources
$(BUILD_DIR)/%.o: $(CURRENT_DIR)/%.c
	@echo "Compiling $<..."
	$(CC) -I $(KERNEL_INCLUDE) -I $(LIBC_INCLUDE) -I $(INTERFACE_INCLUDE) -c $< -o $@ $(CFLAGS) -O2 -Wall -Wextra

# Assemble ASM sources
$(BUILD_DIR)/%.o: $(CURRENT_DIR)/%.asm
	@echo "Assembling $<..."
	$(AS) -f elf $< -o $@

# Create build directory if not exists
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Clean build directory
clean:
	rm -f $(OBJECTS)

# Phony targets
.PHONY: all clean
