# $@ = target file
# $< = first dependency
# $^ = all dependencies

BOOT_STAGE_INCLUDE = boot/common
BOOT_STAGE2_INCLUDE = boot/stage2/includes

all: build_dir boot disk.img run
#boot run

build_dir:
	mkdir -p build

stage1.bin: boot/stage1/stage1.asm
	nasm -f bin -I $(BOOT_STAGE_INCLUDE) -o build/$@ $<

stage2.bin: boot/stage2/stage2.asm
	nasm -f bin -I $(BOOT_STAGE_INCLUDE) -I $(BOOT_STAGE2_INCLUDE) -o build/$@ $<

kernel_entry.bin: kernel/kernel_entry.asm
	nasm -f bin -I $(BOOT_STAGE2_INCLUDE) -o build/$@ $<

# concatenate the both stages of bootloader
boot: stage1.bin stage2.bin kernel_entry.bin
	cat build/stage1.bin build/stage2.bin build/kernel_entry.bin > build/boot.bin

disk.img: stage1.bin stage2.bin kernel_entry.bin
	dd if=/dev/zero of=build/disk.img bs=512 count=2880
	dd if=build/stage1.bin of=build/disk.img bs=512 seek=0 conv=notrunc
	dd if=build/stage2.bin of=build/disk.img bs=512 seek=1 conv=notrunc
	dd if=build/kernel_entry.bin of=build/disk.img bs=512 seek=59 conv=notrunc

run:
	# qemu-system-x86_64 -drive  format=raw,file=build/boot.bin
	qemu-system-x86_64 -drive  format=raw,file=build/disk.img
	#,if=ide,index=0,media=disk

# Clean up generated files
clean:
	rm -rf build
