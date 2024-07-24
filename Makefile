# $@ = target file
# $< = first dependency
# $^ = all dependencies

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
	mkdir -p $(BUILD_DIR)

# stage 1 (binary)
stage1.bin: boot/stage1/stage1.asm
	nasm -f bin -I $(BOOT_STAGE_INCLUDE) -o build/$@ $<

# stage 2 (elf)
stage2.elf: boot/stage2/stage2.asm
	nasm -f elf32 -I $(BOOT_STAGE_INCLUDE) -I $(BOOT_STAGE2_INCLUDE) -o build/$@ $<

# stage 2 (c)
elf.elf: boot/stage2/elf.c
	gcc -m32 -fno-pie -ffreestanding -I $(BOOT_STAGE2_C_INCLUDE) -I $(BOOT_STAGE2_C_STD_INCLUDE) -c boot/stage2/elf.c -o build/elf.elf
mem.elf: boot/stage2/mem.c
	gcc -m32 -fno-pie -ffreestanding -I $(BOOT_STAGE2_C_INCLUDE) -I $(BOOT_STAGE2_C_STD_INCLUDE) -c boot/stage2/mem.c -o build/mem.elf
print.elf: boot/stage2/print.c
	gcc -m32 -fno-pie -ffreestanding -I $(BOOT_STAGE2_C_INCLUDE) -I $(BOOT_STAGE2_C_STD_INCLUDE) -c $< -o build/print.elf
port_io.elf: boot/stage2/port_io.c
	gcc -m32 -fno-pie -ffreestanding -I $(BOOT_STAGE2_C_INCLUDE) -I $(BOOT_STAGE2_C_STD_INCLUDE) -c boot/stage2/port_io.c -o build/port_io.elf
ata.elf: boot/stage2/ata.c
	gcc -m32 -fno-pie -ffreestanding -I $(BOOT_STAGE2_C_INCLUDE) -I $(BOOT_STAGE2_C_STD_INCLUDE) -c boot/stage2/ata.c -o build/ata.elf
ISO9660.elf: boot/stage2/ISO9660.c
	gcc -m32 -fno-pie -ffreestanding -I $(BOOT_STAGE2_C_INCLUDE) -I $(BOOT_STAGE2_C_STD_INCLUDE) -c boot/stage2/ISO9660.c -o build/ISO9660.elf

stage2.bin: stage2.elf ata.elf elf.elf print.elf port_io.elf mem.elf ISO9660.elf
	# ld -m elf_i386 -Ttext 0x0500 --oformat binary -o build/stage2.bin build/stage2.elf build/elf.elf build/print.elf
	ld -m elf_i386 -T boot/stage2/stage2.ld --oformat binary -o build/stage2.bin build/stage2.elf build/elf.elf build/print.elf build/port_io.elf build/ata.elf build/mem.elf build/ISO9660.elf

# build kernel
kernel.elf:
	@echo "building kernel"
	$(MAKE) -C kernel

# ISO9660
image.iso: stage1.bin stage2.bin kernel.elf
	mkdir -p $(ISO_DIR)/
	cp $(BUILD_DIR)/stage1.bin $(ISO_DIR)/
	cp ab.txt $(ISO_DIR)/
	cp $(BUILD_DIR)/stage2.bin $(ISO_DIR)/
	cp kernel/build/kernel_entry.bin $(ISO_DIR)/

	xorriso -as mkisofs -R -J -b stage1.bin -no-emul-boot -boot-load-size 4 -o $@ $(ISO_DIR)


# write all stages to the disk image
disk.img: stage1.bin stage2.bin kernel.elf
	dd if=/dev/zero of=build/disk.img bs=512 count=2880
	dd if=build/stage1.bin of=build/disk.img bs=512 seek=0 conv=notrunc
	dd if=build/stage2.bin of=build/disk.img bs=512 seek=1 conv=notrunc
	dd if=kernel/build/kernel_entry.bin of=build/disk.img bs=512 seek=59 conv=notrunc

# Test the disk image using emulator
run:
	# from disk image
	# qemu-system-x86_64 -drive  format=raw,file=build/disk.img
	
	# from iso image
	qemu-system-x86_64 -cdrom image.iso

# Clean up generated files
clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(ISO_DIR)
	$(MAKE) -C kernel clean
