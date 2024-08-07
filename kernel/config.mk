# config.mk

# Define the compiler and assembler
CC = gcc
AS = nasm

# Common flags
ASMFLAGS = -f elf32
CFLAGS = -m32 -fno-pie -ffreestanding -Wall -Wextra
LDFLAGS = -m elf_i386

# Include directories relative to the root directory
KERNEL_INCLUDE = $(ROOT_DIR)/includes
BOOT_STAGE2_INCLUDE = $(ROOT_DIR)/../boot/stage2/includes
