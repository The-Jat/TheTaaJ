#include <stdint.h>
#include <arch/x86/cpu.h>
#include <defs.h>

static CpuInformation_t CpuInformation = { 0 };
static int CpuInitialized = 0;

/* CpuInitialize
 * Initializes the CPU and gathers available
 * information about it */
void 
CpuInitialize(void)
{
	// Variables
	uint32_t eax, ebx, ecx, edx;

	// Has init already run?
	if (CpuInitialized != 1) {

		// Get base cpu-id information
		CpuId(0, &eax, &ebx, &ecx, &edx);

		// Store cpu-id level
		CpuInformation.CpuIdLevel = eax;

		// Does it support retrieving features?
		if (CpuInformation.CpuIdLevel >= 1) {
			CpuId(1, &eax, &ebx, &ecx, &edx);
			CpuInformation.EcxFeatures = ecx;
			CpuInformation.EdxFeatures = edx;
		}

		// Get extensions supported
		CpuId(0x80000000, &eax, &ebx, &ecx, &edx);

		// Store them
		CpuInformation.CpuIdExtensions = eax;
	}

	// Can we enable FPU?
	if (CpuInformation.EdxFeatures & CPUID_FEAT_EDX_FPU) {
		CpuEnableFpu();
	}

	// Can we enable SSE?
	if (CpuInformation.EdxFeatures & CPUID_FEAT_EDX_SSE) {
		CpuEnableSse();
	}
}
