#ifndef __INTERRUPTS_H_
#define __INTERRUPTS_H_

#include <defs.h>
#include <os/driver/interrupt.h>

/* Structures */
typedef struct _InterruptDescriptor {
	Interrupt_t					Interrupt;
	UUId_t                      Id;
	UUId_t						Ash;
	UUId_t						Thread;
	Flags_t						Flags;
	int							Source;
	struct _InterruptDescriptor	*Link;
} InterruptDescriptor_t;

/* InterruptInitialize
 * Initializes the interrupt-manager code
 * and initializes all the resources for
 * allocating and freeing interrupts */
__EXTERN void InterruptInitialize(void);

#endif