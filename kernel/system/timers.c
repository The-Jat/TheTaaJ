/* 
 * Timer Mangement Interface
 * - Contains the timer interface system and its implementation
 *   keeps track of system timers
 */
#define __MODULE		"TIMR"
//#define __TRACE

/* Includes 
 * - System */
#include <interrupts.h>
// #include <scheduler.h>
#include <ds/list.h>
#include <timers.h>
//#include <debug.h>
#include <heap.h>

/* Includes
 * - Library */
#include <libc/stdlib.h>
#include <stddef.h>

// For logging
#include <video/video.h>

/* Globals */
static MCoreSystemTimer_t *GlbActiveSystemTimer = NULL;
static List_t *GlbSystemTimers = NULL;
static List_t *GlbTimers = NULL;
static UUId_t GlbTimerIds = 0;
static int GlbTimersInitialized = 0;

/* TimersInitialize
 * Initializes the timer sub-system that supports
 * registering of system timers and callback timers */
void
TimersInitialize(void)
{
	LogInformation("Timers", "Initializing");
	// Initialize all the globals 
	// including lists etc
	GlbActiveSystemTimer = NULL;
	GlbSystemTimers = ListCreate(KeyInteger, LIST_SAFE);
	GlbTimers = ListCreate(KeyInteger, LIST_SAFE);
	GlbTimersInitialized = 1;
	GlbTimerIds = 0;
}

UUId_t
TimersCreateTimer(
	TimerHandler_t Callback,
	void *Args, 
	MCoreTimerType_t Type, 
	size_t Timeout)
{
	/* Variables */
	MCoreTimer_t *TimerInfo;
	DataKey_t Key;
	UUId_t Id;

	/* Allocate */
	TimerInfo = (MCoreTimer_t*)kmalloc(sizeof(MCoreTimer_t));
	TimerInfo->Callback = Callback;
	TimerInfo->Args = Args;
	TimerInfo->Type = Type;
	TimerInfo->PeriodicMs = Timeout;
	TimerInfo->MsLeft = (ssize_t)Timeout;

	/* Append to list */
	Key.Value = (int)GlbTimerIds;
	ListAppend(GlbTimers, ListCreateNode(Key, Key, TimerInfo));

	/* Increase */
	Id = GlbTimerIds;
	GlbTimerIds++;

	/* Done */
	return Id;
}

/* Destroys and removes a timer */
void TimersDestroyTimer(UUId_t TimerId)
{
	/* Variables */
	MCoreTimer_t *Timer = NULL;
	DataKey_t Key;

	/* Get Node */
	Key.Value = (int)TimerId;
	Timer = (MCoreTimer_t*)ListGetDataByKey(GlbTimers, Key, 0);

	/* Sanity */
	if (Timer == NULL)
		return;

	/* Remove By Id */
	ListRemoveByKey(GlbTimers, Key);

	/* Free */
	kfree(Timer);
}

/* Sleep function */
void SleepMs(size_t MilliSeconds)
{
	/* Redirect to the scheduler functionality
	 * it allows us to sleep with time-out */
// TODO uncomment later on when schedular is implemented.
	//SchedulerSleepThread(NULL, MilliSeconds);
}

/* Stall functions */
void StallMs(size_t MilliSeconds)
{
// TODO: implement it later.
//	DelayMs(MilliSeconds);
}

/* TimersTick
 * This method actually applies the new tick-delta to all
 * active timers registered, and decreases the total */
void TimersTick(size_t Tick)
{
	// Variables
	ListNode_t *i = NULL;
	size_t MilliTicks = 0;

	// Calculate how many milliseconds
	MilliTicks = DIVUP(Tick, NSEC_PER_MSEC);

	// Update scheduler with the milliticks
// TODO: uncomment later on when schedular is implemented.
//	SchedulerApplyMs(MilliTicks);

	// Now loop through timers registered
	_foreach(i, GlbTimers) {
		MCoreTimer_t *Timer = (MCoreTimer_t*)i->Data;
		Timer->MsLeft -= MilliTicks;
		if (Timer->MsLeft <= 0) {
// TODO: uncomment later on when threading is implemented.		
//			ThreadingCreateThread("Timer Callback", Timer->Callback, Timer->Args, 0);
			if (Timer->Type == TimerPeriodic) {
				Timer->MsLeft = (ssize_t)Timer->PeriodicMs;
			}
			else {
				ListRemoveByNode(GlbTimers, i);
				kfree(Timer);
				kfree(i);
			}
		}
	}
}

/* TimersRegistrate
 * Registrates a interrupt timer source with the
 * timer management, which keeps track of which interrupts
 * are available for time-keeping */
OsStatus_t TimersRegister(UUId_t Source, size_t TickNs)
{
	// Variables
	InterruptDescriptor_t *Interrupt = NULL;
	MCoreSystemTimer_t *SystemTimer = NULL;
	DataKey_t tKey;
	int Delta = abs(1000 - (int)TickNs);

	// Trace
	LogInformation("TimersRegister()");

	// Do some validation about the timer source 
	// the only system timers we want are fast_interrupts
	Interrupt = InterruptGet(Source);
	if (Interrupt == NULL
		|| (Interrupt->Flags & (INTERRUPT_FAST | INTERRUPT_KERNEL)) == 0) {
		LogInformation("Interrupt was not found for source %u", Source);
		return Error;
	}

	// Create a new instance of a system timer
	SystemTimer = (MCoreSystemTimer_t*)kmalloc(sizeof(MCoreSystemTimer_t));
	SystemTimer->Source = Source;
	SystemTimer->Tick = TickNs;
	SystemTimer->Ticks = 0;

	// Add the new timer to the list
	tKey.Value = 0;
	ListAppend(GlbSystemTimers, ListCreateNode(
		tKey, tKey, SystemTimer));

	// Ok, for a system timer we want something optimum
	// of 1 ms per interrupt
	if (GlbActiveSystemTimer != NULL) {
		int ActiveDelta = abs(1000 - GlbActiveSystemTimer->Tick);
		if (ActiveDelta > Delta) {
			GlbActiveSystemTimer = SystemTimer;
		}
	}
	else {
		GlbActiveSystemTimer = SystemTimer;
	}

	// Trace
	LogInformation("New system timer: %u", GlbActiveSystemTimer->Source);

	// Done
	return Success;
}

/* TimersInterrupt
 * Called by the interrupt-code to tell the timer-management system
 * a new interrupt has occured from the given source. This allows
 * the timer-management system to tell us if that was the active
 * timer-source */
OsStatus_t TimersInterrupt(UUId_t Source)
{
	// Sanitize if the source is ok
	if (GlbActiveSystemTimer != NULL) {
		if (GlbActiveSystemTimer->Source == Source) {
			TimersTick(GlbActiveSystemTimer->Tick);
			return Success;
		}
	}

	// Return error
	return Error;
}

