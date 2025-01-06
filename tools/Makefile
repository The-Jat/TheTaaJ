.PHONY: all mkfs clean run create_disk

all: build_dir build/mkfs create_disk

mkfs: build_dir build/mkfs
	@echo "mkfs built successfully."

build_dir:
	mkdir -p build

build/mkfs: mkfs.c
	gcc $< -o $@

create_disk:
	dd if=/dev/zero of=disk.img bs=1M count=128
	./build/mkfs filesystem disk.img

clean:
	rm -rf build
	rm -rf *.img
