#include <idt.h>
#include <kernel_utilities.h>
#include <string.h>

/*
 * IDT Entry
 */
// IDT entry structure 8 bytes
struct idt_entry {
    unsigned short base_low;    // Offset bits 0-15
    unsigned short sel;      // Code segment selector
    unsigned char zero;           // Always 0
    unsigned char flags;      // Type and attributes
    unsigned short base_high;   // Offset bits 16-31
} __attribute__((packed));


struct idt_ptr {
	unsigned short limit;      // Size of the IDT - 1
	unsigned int base;         // Base address of the IDT
} __attribute__((packed));


struct idt_entry idt[256];    // Array to store 256 IDT entries
struct idt_ptr idtp;          // Pointer to the IDT

extern void idt_load();       // Assembly function to load the IDT


/*
 * idt_set_gate
 * Set an IDT gate with the provided parameters.

* Present (P) bit: Bit 7. This must be set to 1, indicating that the interrupt gate is present.
* Descriptor Privilege Level (DPL): Bits 6-5. This specifies the privilege level required to access this interrupt. For kernel-level interrupts, this is typically set to 0.
* Storage Segment (S) bit: Bit 4. This must be set to 0 for interrupt and trap gates.
* Gate Type: Bits 3-0. This specifies the type of gate. For 32-bit interrupt gates, this should be set to 0b1110 (14).

Combining these fields, a typical flags value for a kernel-level interrupt gate (privilege level 0) would be 0x8E:

** Present: 1 (bit 7)
** DPL: 00 (bits 6-5)
** S: 0 (bit 4)
** Gate Type: 1110 (bits 3-0)
 */
void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags) {
	idt[num].base_low =     (base & 0xFFFF);              // Set lower 16 bits of base address
	idt[num].base_high =    (base >> 16) & 0xFFFF;        // Set upper 16 bits of base address
	idt[num].sel =          sel;                          // Set segment selector
	idt[num].zero =         0;                            // Always zero
	idt[num].flags =        flags;                        // Set flags (type and attributes)
}


/*
 * idt_install
 * Install the IDTs by setting up the IDT pointer, clearing the IDT, and loading the IDT.
 */
void idt_install() {
	idtp.limit = (sizeof(struct idt_entry) * 256) - 1;	// Calculate the limit of the IDT
	idtp.base = (unsigned int)&idt;		// set the base address of the IDT to the address of 'idt'

	memset(&idt, 0, sizeof(struct idt_entry) * 256); // Clear the IDT entries

	idt_load();	// Load the IDT by calling the assembly function.
}
