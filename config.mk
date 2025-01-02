# config.mk

# Color definitions
RED    := \033[31m 
GREEN  := \033[32m # For instructions
YELLOW := \033[33m # Linker
BLUE   := \033[34m # ASSEMBLY
MAGENTA := \033[35m # C
CYNA   := \033]36m
RESET  := \033[0m

# Define the compiler and assembler
CC = gcc
AS = nasm

# Common flags
ASMFLAGS = -f elf32
CFLAGS = -m32 -fno-pie -nostdlib -ffreestanding -Wall -Wextra -pedantic
LDFLAGS = -m elf_i386

# Include directories relative to the root directory
KERNEL_INCLUDE = $(ROOT_DIR)/kernel/includes
BOOT_STAGE2_INCLUDE = $(ROOT_DIR)/boot/stage2/includes
LIBC_INCLUDE = $(ROOT_DIR)/kernel/includes/libc
INTERFACE_INCLUDE = $(ROOT_DIR)/lib/include
USERSPACE_INCLUDE = $(ROOT_DIR)/userspace/development/include
