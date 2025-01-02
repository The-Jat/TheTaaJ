# libc/Makefile

# Ensure ROOT_DIR is set, defaulting to one level up if not provided
ROOT_DIR ?= $(abspath ..)

# Include the configuration file
include $(ROOT_DIR)/config.mk

# Define directories
BUILD_DIR = $(ROOT_DIR)/build
SUBDIRS = stdlib string
CURRENT_DIR = .

# Define source files
C_SOURCES =

# Define object files
C_OBJECTS = $(C_SOURCES:%.c=$(BUILD_DIR)/%.o)
OBJECTS = $(C_OBJECTS)

# Build all
all: $(BUILD_DIR) $(SUBDIRS) $(OBJECTS)

# Recursively call make in each subdirectory
$(SUBDIRS):
	$(MAKE) -C $@ ROOT_DIR=$(ROOT_DIR)

# Compile C sources
$(BUILD_DIR)/%.o: $(CURRENT_DIR)/%.c
	@echo "Compiling $<..."
	$(CC) -I $(KERNEL_INCLUDE) -I $(LIBC_INCLUDE) -I $(INTERFACE_INCLUDE) -I $(USERSPACE_INCLUDE) -c $< -o $@ $(CFLAGS) -O2 -Wall -Wextra

# Create build directory if not exists
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Clean build directory
clean:
	rm -f $(OBJECTS)

# Phony targets
.PHONY: all clean $(SUBDIRS)
