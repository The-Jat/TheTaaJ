# $@ = target file
# $< = first dependency
# $^ = all dependencies

BOOT_STAGE_INCLUDE = boot/common
BOOT_STAGE2_INCLUDE = boot/stage2/includes

KERNEL_INCLUDES = kernel/idt

all: build_dir disk.img run

# create the build directory
build_dir:
	mkdir -p build

# stage 1 (binary)
stage1.bin: boot/stage1/stage1.asm
	nasm -f bin -I $(BOOT_STAGE_INCLUDE) -o build/$@ $<

# stage 2 (binary)
stage2.bin: boot/stage2/stage2.asm
	nasm -f bin -I $(BOOT_STAGE_INCLUDE) -I $(BOOT_STAGE2_INCLUDE) -o build/$@ $<

# build kernel
kernel.rule:
	@echo "building kernel"
	$(MAKE) -C kernel

# kernel_entry (ELF)
kernel_entry.elf: kernel/kernel_entry.asm
	nasm -f elf -I $(BOOT_STAGE2_INCLUDE) -I $(KERNEL_INCLUDES) -o build/$@ $<

# kernel_main C (ELF)
kernel_main.elf:
	gcc -m32 -fno-pie -ffreestanding -c kernel/kernel_main.c -o build/kernel_main.elf

# kernel.bin linked (BIN)
kernel.bin: kernel kernel_entry.elf kernel_main.elf
	# ld -m elf_i386 -Ttext 0xB000 --oformat binary -o build/kernel.bin build/kernel_entry.elf build/kernel_main.elf
	ld -m elf_i386 -Ttext 0xB000 --oformat binary -o build/kernel.bin kernel/build/kernel_entry.o kernel/build/kernel_main.o

# write all stages to the disk image
disk.img: stage1.bin stage2.bin kernel.rule
	dd if=/dev/zero of=build/disk.img bs=512 count=2880
	dd if=build/stage1.bin of=build/disk.img bs=512 seek=0 conv=notrunc
	dd if=build/stage2.bin of=build/disk.img bs=512 seek=1 conv=notrunc
	# dd if=build/kernel.bin of=build/disk.img bs=512 seek=59 conv=notrunc
	dd if=kernel/build/kernel_entry.o of=build/disk.img bs=512 seek=59 conv=notrunc

# Test the disk image using emulator
run:
	qemu-system-x86_64 -drive  format=raw,file=build/disk.img

# Clean up generated files
clean:
	rm -rf build
	$(MAKE) -C kernel clean
