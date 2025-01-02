/* 
 * MollenOS MCore - Spinlock Support Definitions & Structures
 * - This header describes the base spinlock-structures, prototypes
 *   and functionality, refer to the individual things for descriptions
 */

/* Includes 
 * - System */
#include <os/spinlock.h>
//#include <os/syscall.h>

/* Includes 
 * - Library */
#include <stddef.h>

/* Externs
 * Access to platform specifics */
__EXTERN int _spinlock_acquire(Spinlock_t *Spinlock);
__EXTERN int _spinlock_test(Spinlock_t *Spinlock);
__EXTERN void _spinlock_release(Spinlock_t *Spinlock);

/* SpinlockReset
 * This initializes a spinlock
 * handle and sets it to default
 * value (unlocked) */
OsStatus_t SpinlockReset(Spinlock_t *Lock)
{
	/* Sanity */
	if (Lock == NULL) {
		return Error;
	}

	/* Reset it to 0 */
	*Lock = SPINLOCK_INIT;
	return Success;
}

/* SpinlockAcquire
 * Acquires the spinlock while busy-waiting
 * for it to be ready if neccessary */
OsStatus_t SpinlockAcquire(Spinlock_t *Lock)
{
	/* Sanitize params */
	if (Lock == NULL) {
		return Error;
	}

	/* Redirect to platform specific implementation */
	return _spinlock_acquire(Lock) == 1 ? Success : Error;
}

/* SpinlockTryAcquire
 * Makes an attempt to acquire the
 * spinlock without blocking */
OsStatus_t SpinlockTryAcquire(Spinlock_t *Lock)
{
	/* Sanity */
	if (Lock == NULL) {
		return Error;
	}

	/* Redirect to platform specific implementation */
	return _spinlock_test(Lock) == 1 ? Success : Error;
}

/* SpinlockRelease
 * Releases the spinlock, and lets
 * other threads access the lock */
OsStatus_t SpinlockRelease(Spinlock_t *Lock)
{
	/* Sanity */
	if (Lock == NULL) {
		return Error;
	}

	/* Redirect to platform specific implementation */
	_spinlock_release(Lock);
	return Success;
}

