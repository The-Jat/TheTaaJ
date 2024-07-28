.PHONY: all ISO9660 elf clean run

all: build_dir build/Read9660 build/elf

ISO9660: build_dir build/Read9660
	@echo "Built ISO9660 utility."

elf: buld_dir build/elf

build_dir:
	mkdir -p build

build/Read9660: Read9660.c
	gcc $< -o $@

build/elf: elf.c
	gcc $< -o $@

run_iso9660:
	./build/Read9660 ../image.iso

clean:
	rm -rf build

