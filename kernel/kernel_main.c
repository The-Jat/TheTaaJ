#include <vga_print.h>
#include <boot/boot_datastructure.h>
#include <defs.h>
#include <video/boot_terminal.h>
#include <log/log.h>

// GDT
#include <arch/x86/x32/gdt.h>


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

	int abc = 1234567;
	unsigned int u_int = 987654321;
	const char* str = "Testing String";
	LogInformation("k_main", "Testing LogInformation, string = %s, integer = %d", str, abc);

	Log("Log, abc = %d", abc);
	LogRaw("LogRaw abc = %d", abc);
	LogDebug("k_main", "LogDebug\n");
	LogFatal("k_main", "LogFatal");

	// Initialize the GDT
	GdtInitialize();
	
	// Initialize the system.
	// Initialize(&x86BootInfo);

// Infinite loop
	while(1){}

}
