# vga/Makefile

# Ensure ROOT_DIR is set, defaulting to one level up if not provided
ROOT_DIR ?= $(abspath ..)

# Include the configuration file
include $(ROOT_DIR)/config.mk

# Define directories
BUILD_DIR = $(ROOT_DIR)/build
VGA_DIR = .

# Define source files
C_SOURCES = vga.c

# Define object files
C_OBJECTS = $(C_SOURCES:%.c=$(BUILD_DIR)/%.o)

# Build all
all: $(BUILD_DIR) $(C_OBJECTS)

# Compile C sources
$(BUILD_DIR)/%.o: $(VGA_DIR)/%.c
	@echo "Compiling $<..."
	$(CC) -I $(KERNEL_INCLUDE) -c $< -o $@ $(CFLAGS) -O2 -Wall -Wextra

# Create build directory if not exists
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Clean build directory
clean:
	rm -f $(C_OBJECTS)

# Phony targets
.PHONY: all clean
