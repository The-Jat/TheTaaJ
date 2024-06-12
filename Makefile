# $@ = target file
# $< = first dependency
# $^ = all dependencies

BOOT_STAGE_INCLUDE = boot/common

all: build_dir boot run

build_dir:
	mkdir -p build

stage1.bin: boot/stage1/stage1.asm
	nasm -f bin -I $(BOOT_STAGE_INCLUDE) -o build/$@ $<

stage2.bin: boot/stage2/stage2.asm
	nasm -f bin -I $(BOOT_STAGE_INCLUDE) -o build/$@ $<

# concatenate the both stages of bootloader
boot: stage1.bin stage2.bin
	cat build/stage1.bin build/stage2.bin > build/boot.bin

run:
	qemu-system-x86_64 -drive  format=raw,file=build/boot.bin 

# Clean up generated files
clean:
	rm -rf build
