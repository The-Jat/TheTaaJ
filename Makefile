# $@ = target file
# $< = first dependency
# $^ = all dependencies

all: build_dir stage1.bin

build_dir:
	mkdir -p build

stage1.bin: boot/stage1/stage1.asm
	nasm -f bin -o build/$@ $<

run:
	qemu-system-x86_64 -drive  format=raw,file=build/stage1.bin 

# Clean up generated files
clean:
	rm -rf build
