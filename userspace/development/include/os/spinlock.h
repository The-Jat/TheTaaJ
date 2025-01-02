/*
 * MollenOS MCore - Spinlock Support Definitions & Structures
 * - This header describes the base spinlock-structures, prototypes
 *   and functionality, refer to the individual things for descriptions
 */

#ifndef _SPINLOCK_INTERFACE_H_
#define _SPINLOCK_INTERFACE_H_

/* Includes
 * - System */
#include <defs.h>

/* The definition of a spinlock handle
 * used for primitive lock access */
typedef int Spinlock_t;
#define SPINLOCK_INIT			0

/* Start one of these before function prototypes */

/* SpinlockReset
 * This initializes a spinlock
 * handle and sets it to default
 * value (unlocked) */
OsStatus_t SpinlockReset(Spinlock_t *Lock);

/* SpinlockAcquire
 * Acquires the spinlock while busy-waiting
 * for it to be ready if neccessary */
OsStatus_t SpinlockAcquire(Spinlock_t *Lock);

/* SpinlockTryAcquire
 * Makes an attempt to acquire the
 * spinlock without blocking */
OsStatus_t SpinlockTryAcquire(Spinlock_t *Lock);

/* SpinlockRelease
 * Releases the spinlock, and lets
 * other threads access the lock */
OsStatus_t SpinlockRelease(Spinlock_t *Lock);

#endif //!_SPINLOCK_INTERFACE_H_

