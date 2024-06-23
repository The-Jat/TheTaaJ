#include "system.h"

/*
 * Exception Handlers, defined in assembly
 */
extern void _isr0();
extern void _isr1();
extern void _isr2();
extern void _isr3();
extern void _isr4();
extern void _isr5();
extern void _isr6();
extern void _isr7();
extern void _isr8();
extern void _isr9();
extern void _isr10();
extern void _isr11();
extern void _isr12();
extern void _isr13();
extern void _isr14();
extern void _isr15();
extern void _isr16();
extern void _isr17();
extern void _isr18();
extern void _isr19();
extern void _isr20();
extern void _isr21();
extern void _isr22();
extern void _isr23();
extern void _isr24();
extern void _isr25();
extern void _isr26();
extern void _isr27();
extern void _isr28();
extern void _isr29();
extern void _isr30();
extern void _isr31();


void isrs_install() {
	/* Exception Handlers */
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
