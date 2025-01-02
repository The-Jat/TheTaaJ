/*
 * MollenOS MCore - Timer Support Definitions & Structures
 * - This header describes the base timer-structures, prototypes
 *   and functionality, refer to the individual things for descriptions
 */

#ifndef _TIMERS_INTERFACE_H_
#define _TIMERS_INTERFACE_H_

/* Includes 
 * - System */
#include <defs.h>

/* Timer callback definition, the
 * function proto-type for a timer callback */
typedef void(*TimerHandler_t)(void*);

/* Timer information flags on creation, 
 * allows customization and timer settings */
#define	TIMER_PERIODIC				0x1
#define TIMER_FAST					0x2

#endif //!_TIMERS_INTERFACE_H_

