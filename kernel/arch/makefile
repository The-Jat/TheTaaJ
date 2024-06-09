# Makefile for building the mollenos kernel image
# Includes sub-libraries as
# - acpica
# - arch sub-layer
INCLUDES = -I../include
arch = i386
ifeq ($(arch),i386)
    INCLUDES += -Ix86 -Ix86/x32
    SOURCES = 		$(wildcard x86/x32/*.c) \
			  $(wildcard x86/*.c)
    ASSEMBLY = $(wildcard x86/x32/*.s)
	ASFLAGS = -f elf32
	HPATH = x86/x32/arch.h
else

endif

OBJECTS = $(ASSEMBLY:.s=.o) $(SOURCES:.c=.o)
CFLAGS = $(GCFLAGS) -D_KAPI -DLIBC_KERNEL -Wno-address-of-packed-member $(INCLUDES)
LFLAGS = -m elf_i386

.PHONY: all
all: header ../build/$(arch).lib

.PHONY: header
header:
	# cp $(HPATH) ../include/arch.h

../build/i386.lib: $(OBJECTS)
	echo $(LD)
	echo $(LD) $(LFLAGS) $(OBJECTS) -o $@
	pwd

i386.lib: $(OBJECTS)
	$(LD) $(LFLAGS) $(OBJECTS) -o $@

%.o : %.c
	$(CC) -c $(CFLAGS) -o $@ $<

%.o : %.s
	nasm $(ASFLAGS) $< -o $@

.PHONY: clean
clean:
	rm -f ../include/arch.h
	rm -f ../build/$(arch).lib
	rm -f $(OBJECTS)
