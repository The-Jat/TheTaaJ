#include <defs.h>
#include <system/address_space.h>
/* Includes
 * - Library
 */
#include <stddef.h>
#include <string.h>

/* Globals */
static AddressSpace_t g_KernelAddressSpace;

/* AddressSpaceInitKernel
 * Initializes the Kernel Address Space 
 * This only copies the data into a static global
 * storage, which means users should just pass something
 * temporary structure
 */
OsStatus_t AddressSpaceInitKernel(AddressSpace_t *Kernel)
{
	// Sanitize parameter
//	assert(Kernel != NULL);

	// Copy data into our static storage
	g_KernelAddressSpace.Cr3 = Kernel->Cr3;
	g_KernelAddressSpace.Flags = Kernel->Flags;
	g_KernelAddressSpace.PageDirectory = Kernel->PageDirectory;

	// Setup reference and lock
//	SpinlockReset(&g_KernelAddressSpace.Lock);
	g_KernelAddressSpace.References = 1;

	// No errors
	return Success;
}
