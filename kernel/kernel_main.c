#include <vga.h>

// C kernel entry.
void k_main(){
	init_video();
	puts("This is the Kernel", VGA_COLOR_LIGHT_RED, VGA_COLOR_LIGHT_GREY);

// Infinite loop
	while(1){}

}
