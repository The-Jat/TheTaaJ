# Makefile for building the mollenos kernel image
# Includes sub-libraries as
# - acpica
# - arch sub-layer
INCLUDES = -Iinclude
SOURCES = init.c
OBJECTS = $(SOURCES:.c=.o)

CFLAGS = $(GCFLAGS) -m32 -D_KAPI -DUNICODE -DLIBC_KERNEL -D_KRNL_DLL $(INCLUDES)
# LFLAGS = /nodefaultlib /subsystem:native /entry:kentry /fixed /base:"0x100000" /manifest:no build/i386.lib
LFLAGS = -m elf_i386 -nostdlib -Ttext 0x100000 -e kentry  #build/i386.lib

.PHONY: all
all: build arch build/syskrnl.mos

build:
	@mkdir -p $@

.PHONY: arch
arch:
	$(MAKE) -C arch -f makefile

build/syskrnl.mos: $(OBJECTS)
	# $(LD) $(LFLAGS) $(OBJECTS) /out:$@
	# cp arch/x86/x32/boot.o build/
	ld $(LFLAGS) $(OBJECTS) arch/x86/x32/boot.o -o $@
	
%.o : %.c
	$(CC) -c $(CFLAGS) -o $@ $<

.PHONY: clean
clean:
	$(MAKE) -C arch -f makefile clean
	rm -f $(OBJECTS)
	rm -rf build
