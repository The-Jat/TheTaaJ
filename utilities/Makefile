.PHONY: all ISO9660 clean run

all: build_dir build/Read9660

ISO9660: build_dir build/Read9660

build_dir:
	mkdir -p build

build/Read9660: Read9660.c
	gcc $< -o $@

run:
	./build/Read9660 ../image.iso
clean:
	rm -rf build

