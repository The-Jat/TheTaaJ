#include <arch/x86/x32/context.h>
#include <log/log.h>


/* InterruptEntry
 * The common entry point for interrupts, all
 * non-exceptions will enter here, lookup a handler
 * and execute the code
 */
void InterruptEntry(Context_t *Registers) {
// Handle the interrupts
	LogFatal("Interrupts", "InterruptEntry, Interrupt Occurred = %d", Registers->Irq);
	// For the time being, infinity and beyond
	while(1){}
}

/* ExceptionEntry
 * Common entry for all exceptions
 */
void ExceptionEntry(Context_t *Registers) {
// Handle the exceptions
	LogFatal("Interrupts", "ExceptionEntry, Exception Occurred = %d", Registers->Irq);
	// For the time being, infinity and beyond
	while(1){}
}
