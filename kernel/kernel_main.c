#include <vga.h>
#include <datastructure.h>

// C kernel entry.
void k_main(Multiboot_t *BootInfo, OsBootDescriptor* BootDescriptor){
	init_video();
	puts("This is the Kernel main", VGA_COLOR_LIGHT_RED, VGA_COLOR_LIGHT_GREY);

// Infinite loop
	while(1){}

}
