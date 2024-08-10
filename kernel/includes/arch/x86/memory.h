#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <stdint.h>
#include <stddef.h>
#include <defs.h>

/* Memory Map Structure 
 * This is the structure passed to us by
 * the mBoot bootloader */
typedef struct BIOSMemoryRegion {
	uint64_t				Address;
	uint64_t				Size;
	uint32_t				Type;		//1 => Available, 2 => ACPI, 3 => Reserved
	uint32_t				Nil;
	uint64_t				Padding;
} __attribute__((packed)) BIOSMemoryRegion_t;

/* System reserved memory mappings
 * this is to faster/safer map in system
 * memory like ACPI/device memory etc etc */
typedef struct SystemMemoryMapping {
	PhysicalAddress_t		pAddressStart;
	VirtualAddress_t		vAddressStart;
	size_t					Length;
	int						Type;	//Type. 2 - ACPI
} __attribute__((packed)) SystemMemoryMapping_t;

/* MmPhyiscalInit
 * This is the physical memory manager initializor
 * It reads the multiboot memory descriptor(s), initialies
 * the bitmap and makes sure reserved regions are allocated */
OsStatus_t MmPhyiscalInit(void *BootInfo, OsBootDescriptor *Descriptor);

#endif /* __MEMORY_H__ */
