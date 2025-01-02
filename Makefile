# $@ = target file
# $< = first dependency
# $^ = all dependencies

# Define ROOT_DIR before including config.mk
ROOT_DIR := $(abspath .)

# Include the configuration file
include $(ROOT_DIR)/config.mk

# directory variables
BUILD_DIR = build
ISO_DIR = iso

# include variables
BOOT_STAGE_INCLUDE = boot/common
BOOT_STAGE2_INCLUDE = boot/stage2/includes
BOOT_STAGE2_C_INCLUDE = boot/stage2/c_includes
BOOT_STAGE2_C_STD_INCLUDE = boot/stage2/c_includes/std
KERNEL_INCLUDES = kernel/idt

all: build_dir image.iso run

# create the build directory
build_dir:
	@echo "$(YELLOW)Creating build directory...$(RESET)"
	mkdir -p $(BUILD_DIR)

# stage 1 (binary)
stage1.bin: boot/stage1/stage1.asm
	@echo "$(BLUE)Assembling stage1...$(RESET)"
	nasm -f bin -I $(BOOT_STAGE_INCLUDE) -o build/$@ $<

# stage 2 (elf)
stage2.elf: boot/stage2/stage2.asm
	@echo "$(BLUE)Assembling stage2...$(RESET)"
	nasm -f elf32 -I $(BOOT_STAGE_INCLUDE) -I $(BOOT_STAGE2_INCLUDE) -o build/$@ $<

# stage 2 (c)
elf.elf: boot/stage2/elf.c
	@echo "$(MAGENTA)Compiling elf.c...$(RESET)"
	gcc -m32 -fno-pie -ffreestanding -I $(BOOT_STAGE2_C_INCLUDE) -I $(BOOT_STAGE2_C_STD_INCLUDE) -c boot/stage2/elf.c -o build/elf.elf
mem.elf: boot/stage2/mem.c
	@echo "$(MAGENTA)Compiling mem.c...$(RESET)"
	gcc -m32 -fno-pie -ffreestanding -I $(BOOT_STAGE2_C_INCLUDE) -I $(BOOT_STAGE2_C_STD_INCLUDE) -c boot/stage2/mem.c -o build/mem.elf
print.elf: boot/stage2/print.c
	@echo "$(MAGENTA)Compiling stage2/print.c...$(RESET)"
	gcc -m32 -fno-pie -ffreestanding -I $(BOOT_STAGE2_C_INCLUDE) -I $(BOOT_STAGE2_C_STD_INCLUDE) -c $< -o build/print.elf
port_io.elf: boot/stage2/port_io.c
	@echo "$(MAGENTA)Compiling stage2/port_io.c...$(RESET)"
	gcc -m32 -fno-pie -ffreestanding -I $(BOOT_STAGE2_C_INCLUDE) -I $(BOOT_STAGE2_C_STD_INCLUDE) -c boot/stage2/port_io.c -o build/port_io.elf
ata.elf: boot/stage2/ata.c
	@echo "$(MAGENTA)Compiling stage2/ata.c...$(RESET)"
	gcc -m32 -fno-pie -ffreestanding -I $(BOOT_STAGE2_C_INCLUDE) -I $(BOOT_STAGE2_C_STD_INCLUDE) -c boot/stage2/ata.c -o build/ata.elf
ISO9660.elf: boot/stage2/ISO9660.c
	@echo "$(MAGENTA)Compiling stage2/ISO9660.c...$(RESET)"
	gcc -m32 -fno-pie -ffreestanding -I $(BOOT_STAGE2_C_INCLUDE) -I $(BOOT_STAGE2_C_STD_INCLUDE) -c boot/stage2/ISO9660.c -o build/ISO9660.elf

boot_menu.elf: boot/stage2/boot_menu.c
	@echo "$(MAGENTA)Compiling stage2/boot_menu.c...$(RESET)"
	gcc -m32 -fno-pie -ffreestanding -I $(BOOT_STAGE2_C_INCLUDE) -I $(BOOT_STAGE2_C_STD_INCLUDE) -c boot/stage2/boot_menu.c -o build/boot_menu.elf

menu.elf: boot/stage2/menu.c
	@echo "$(MAGENTA)Compiling stage2/menu.c...$(RESET)"
	gcc -m32 -fno-pie -ffreestanding -I $(BOOT_STAGE2_C_INCLUDE) -I $(BOOT_STAGE2_C_STD_INCLUDE) -c boot/stage2/menu.c -o build/menu.elf

stage2.bin: stage2.elf ata.elf elf.elf print.elf port_io.elf mem.elf ISO9660.elf boot_menu.elf menu.elf
	@echo "$(YELLOW)Linking stage2...$(RESET)"
#	ld -m elf_i386 -Ttext 0x0500 --oformat binary -o build/stage2.bin build/stage2.elf build/elf.elf build/print.elf
	ld -m elf_i386 -T boot/stage2/stage2.ld --oformat binary -o build/stage2.bin build/stage2.elf build/elf.elf build/print.elf build/port_io.elf build/ata.elf build/mem.elf build/ISO9660.elf build/boot_menu.elf build/menu.elf

# build kernel
kernel.elf:
	@echo "$(GREEN)Building kernel...$(RESET)"
	$(MAKE) -C kernel

libraries:
	@echo "$(GREEN)Building libraries...$(RESET)"
	$(MAKE) -C lib

# ISO9660
image.iso: stage1.bin stage2.bin libraries kernel.elf
	@echo "$(GREEN)Creating ISO image...$(RESET)"
	mkdir -p $(ISO_DIR)/
	mkdir -p $(ISO_DIR)/boot
	mkdir -p $(ISO_DIR)/kernel
	mkdir -p $(ISO_DIR)/saample
	cp $(BUILD_DIR)/stage1.bin $(ISO_DIR)/
	cp ab.txt $(ISO_DIR)/
	cp sample.txt $(ISO_DIR)/saample/
	cp $(BUILD_DIR)/stage2.bin $(ISO_DIR)/
	cp build/kernel.elf $(ISO_DIR)/kernel

# We will use the ISO Level 3, which supports longer names, and extended features, but less compatibility to old systems.
# If we dont specify the ISO level explicitly then I guess by default is level 1, which has more restrictions but full compatibility to old systems.
# ISO level is specified by option, -iso-level 1 OR 2 OR 3
	xorriso -as mkisofs -R -J -b stage1.bin -iso-level 3 -no-emul-boot -boot-load-size 4 -o $@ $(ISO_DIR)


# write all stages to the disk image
disk.img: stage1.bin stage2.bin kernel.elf
	@echo "$(GREEN)Creating disk image...$(RESET)"
	dd if=/dev/zero of=build/disk.img bs=512 count=2880
	dd if=build/stage1.bin of=build/disk.img bs=512 seek=0 conv=notrunc
	dd if=build/stage2.bin of=build/disk.img bs=512 seek=1 conv=notrunc
	dd if=kernel/build/kernel_entry.bin of=build/disk.img bs=512 seek=59 conv=notrunc

# Test the disk image using emulator
run:
	@echo "$(GREEN)Running the image...$(RESET)"
# from disk image
#	qemu-system-x86_64 -drive  format=raw,file=build/disk.img
	
# from iso image
	qemu-system-x86_64 -m 512M  -cdrom image.iso

# Utilities
utilities:
	$(MAKE) -C utilities

ISO9660_utility_run:
	$(MAKE) -C utilities run_iso9660

# Clean up generated files
clean:
	@echo "$(RED)Cleaning up...$(RESET)"
	rm -rf $(BUILD_DIR)
	rm -rf $(ISO_DIR)
#	remove iso image
	rm -rf image.iso
#	Clean kernel
	$(MAKE) -C kernel clean
#	Clean utilities
	$(MAKE) -C utilities clean


.PHONY: utilities ISO9660_utility_run
