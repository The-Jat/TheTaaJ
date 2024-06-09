# $@ = target file
# $< = first dependency
# $^ = all dependencies

# Include all the definitions for os
include config/common.mk

# Define phony targets to avoid conflicts with files of the same name
.PHONY: all taaj clean

# Default target to build everything and run the OS image
all: build_tools gen_revision build_bootloader build_kernel init_rd deploy create_img run_img

.PHONY: build_tools gen_revision_minor build_bootloader

.PHONY: build_tools
build_tools:
	@$(MAKE) -s -C tools/lzss -f makefile
	@$(MAKE) -s -C tools/rd -f makefile
	@$(MAKE) -s -C tools/diskutility -f makefile
	@$(MAKE) -s -C tools/revision -f makefile
	@$(MAKE) -s -C tools/file2c -f makefile

.PHONY: gen_revision_minor
	@printf "%b" "\033[1;35mUpdating revision version (minor)\033[m\n"
	@./revision minor gcc

# update the revision.h file everytime, this command is run. build count is updated.
.PHONY: gen_revision
gen_revision:
	@printf "%b" "\033[1;35mUpdating revision file\033[m\n"
	@./revision build clang
	# to copy to kernel
	# @cp revision.h kernel/include/revision.h

# build bootloader
build_bootloader:
	$(MAKE) -s -C boot -f makefile

# build kernel
.PHONY: build_kernel
build_kernel:
	$(MAKE) -s -C kernel -f makefile

# ramdisk
.PHONY: init_rd
init_rd:
	# $(MAKE) -s -C librt -f makefile
	

deploy:
	mkdir -p deploy
	mkdir -p deploy/hdd/system
	cp -a boot/build/. deploy/
	./lzss c kernel/build/syskrnl.mos deploy/hdd/system/syskrnl.mos

create_img:
	mono diskutility -auto -target img -scheme mbr

run_img:
	qemu-system-x86_64 -drive format=raw,file=mollenos.img
	
# Rule to assemble the stage1boot.asm file into a binary file stage1boot.bin
stage1boot.bin: fat32.asm
	#nasm $< -f bin -o $@
	nasm -f bin fat32.asm -o bootloader.bin

stage2boot.bin: kernel.asm
	# nasm $< -f bin -o $@
	nasm -f bin kernel.asm -o kernel.bin

os-image.bin: stage1boot.bin stage2boot.bin
	# cat stage1boot.bin stage2boot.bin > os-image.bin
	dd if=/dev/zero of=disk.img bs=1M count=10
	mkfs.vfat -F 32 disk.img
	sudo mount -o loop disk.img /mnt
	sudo dd if=bootloader.bin of=disk.img bs=512 count=1 conv=notrunc
	sudo cp kernel.bin /mnt/KERNEL.BIN
	sudo umount /mnt

stage1boot: stage1.asm
	nasm -f bin stage1.asm -o build/stage1.sys

stage2boot: stage2.asm
	nasm -f bin stage2.asm -o build/stage2.sys

loader: stage1boot stage2boot
	# cat build/stage1.sys build/stage2.sys > build/mollen.bin
	# dd if=/dev/zero of=disk.img bs=1M count=10
	# mkfs.vfat -F 32 disk.img
	# sudo mount -o loop disk.img /mnt
	# sudo dd if=build/stage1.sys of=disk.img bs=512 count=1 conv=notrunc
	#sudo cp kernel.bin /mnt/KERNEL.BIN
	# sudo umount /mnt

taaj: loader
	cp build/stage1.sys diskutility/deploy/stage1.sys
	cp build/stage2.sys diskutility/deploy/stage2.sys
	#qemu-system-x86_64 -drive format=raw,file=disk.img

# Rule to run the OS image using QEMU
run: os-image.bin
	#qemu-system-x86_64 -drive format=raw,file=disk.img
	qemu-system-x86_64 -fda disk.img
	#qemu-system-x86_64 -drive format=raw,file=os-image.bin,if=ide,index=0,media=disk

# Clean up generated files
clean:
	rm -rf *.bin *.o *.obj *.img build/* *.sys
	$(MAKE) -C boot -f makefile clean
	# $(MAKE) -C kernel -f makefile clean
	rm -rf deploy
