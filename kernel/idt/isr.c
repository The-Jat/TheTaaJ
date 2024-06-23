#include "idt.h"

/*
 * Exception Handlers, defined in assembly
 */
extern void _isr0();


void isrs_install() {
	/* Exception Handlers */
	// index = 0, _isr0 = address of ISR, 0x08 = code segment selector, 0x8E = flags
	idt_set_gate(0, (unsigned)_isr0, 0x08, 0x8E);
}

void fault_handler(struct regs *r) {
	if (r->int_no < 32) {
		char* vMem = (char*) 0xb8000;
		vMem[0] = 'E';
		vMem[1] = 0x07;

		vMem[2] = 'x';
		vMem[3] = 0x07;

		vMem[4] = 'c';
		vMem[5] = 0x07;

		vMem[6] = 'e';
		vMem[7] = 0x07;

		vMem[8] = 'p';
		vMem[9] = 0x07;

		vMem[10] = 't';
		vMem[11] = 0x07;

		vMem[12] = 'i';
		vMem[13] = 0x07;

		vMem[14] = 'o';
		vMem[15] = 0x07;

		vMem[16] = 'n';
		vMem[17] = 0x07;


		for (;;);
	}
}
