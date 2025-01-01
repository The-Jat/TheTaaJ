#include <vga_print.h>
#include <boot/boot_datastructure.h>
#include <defs.h>
#include <video/boot_terminal.h>
#include <log/log.h>

// GDT
#include <arch/x86/x32/gdt.h>
// IDT
#include <arch/x86/x32/idt.h>
// PIC
#include <arch/x86/pic.h>
// Physical Memory
#include <arch/x86/memory.h>
// Heap
#include <heap.h>

BootInfo_t x86BootInfo;

// C kernel entry.
void k_main(Multiboot_t *BootInfo, OsBootDescriptor* BootDescriptor){

	// Store the passed arguments value in a global data structure.
	x86BootInfo.ArchBootInfo = (void*)BootInfo;
	x86BootInfo.BootLoaderName = (char*)BootInfo->BootLoaderName;

	x86BootInfo.Descriptor.KernelAddress = BootDescriptor->KernelAddress;
	x86BootInfo.Descriptor.KernelSize = BootDescriptor->KernelSize;
	x86BootInfo.Descriptor.RamDiskAddress = BootDescriptor->RamDiskAddress;
	x86BootInfo.Descriptor.RamDiskSize = BootDescriptor->RamDiskSize;
	x86BootInfo.Descriptor.ExportsAddress = BootDescriptor->ExportsAddress;
	x86BootInfo.Descriptor.ExportsSize = BootDescriptor->ExportsSize;
	x86BootInfo.Descriptor.SymbolsAddress = BootDescriptor->SymbolsAddress;
	x86BootInfo.Descriptor.SymbolsSize = BootDescriptor->SymbolsSize;

	// Initialize cpu
	CpuInitialize();

//	init_video();
//	puts("This is the Kernel main.\n", VGA_COLOR_WHITE, VGA_COLOR_BLACK);

	// Initialize the Boot Terminal
	Initialize_BootTerminal(x86BootInfo.ArchBootInfo);

	int int_type = 1234567;
	unsigned int u_int = 3123456789;
	const char* str = "Testing String";
	void *ptr = (void*) 0xDEADBEEC;
	double float_type = 1234.567;
	LogInformation("k_main", "Testing, string = %s, integer = %d, unsigned int = %u", str, int_type, u_int);
	LogInformation("k_main", "Testing Float, %f", float_type);
	LogInformation("k_main", "Testing Pointer, %p", ptr);

	Log("Log, int_type = %d", int_type);
	LogRaw("LogRaw int_type = %d", int_type);
	LogDebug("k_main", "LogDebug\n");
	LogFatal("k_main", "LogFatal");

	// Print received boot descriptor contents
	LogInformation("k_main", "Kernel Size (in bytes) = %d", BootDescriptor->KernelSize);
	LogInformation("k_main", "Kernel Load Address(hex) = %x", BootDescriptor->KernelAddress);

	// Test Video Text Scrollling
/*	for(int i = 0; i< 20; i++){
		LogInformation("Test", "Line = %d, a b c d e f g h i j k l m n o p q r s t u v w x y z  A B C D E F G H I J K L M N O P Q R S T U V W X Y Z", i);
	}*/
	//while(1){}

	// Initialize the GDT
	GdtInitialize();
	
	// Initialize the IDT
	IdtInitialize();
	// Generate the division by zero exception.
	int a = 0;
	int b = 1;
	//int result = b/a;
	
	// Generate the interrupt manually, for it uncomment below lines.
	// asm volatile (
	// "int $0x35"
	// :
	// :
	// :
	// );

	// Initialize and Remap the PIC
	PicInit();

	// Initialize Physical Memory
	MmPhyiscalInit(BootInfo, BootDescriptor);

	// Initialize Virtual Memory
	MmVirtualInit();

	// Initializing the Heap Memory
	HeapInit();
	// heap_test();

	// Initialize the system.
	// Initialize(&x86BootInfo);

	Log("k_main Kernel Infinite Loop");
// Infinite loop
	while(1){}

}
