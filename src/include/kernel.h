#ifndef _KERNEL_H_
#define _KERNEL_H_

#include <syscalls.h>
#include <semaphore.h>

extern uint32_t Top_Stack;

/**
 * \brief This struct contains function pointers to function to call at
 * different conditions. Theese conditions may not be fatal, but could be.
 * Therefor the handler-functions should be carefull not to do things that
 * rely on the failing functionality.
 *
 * All of these functions are called in privileged mode (system-mode) and
 * one should therefore be carefull to do much more that vital stuff.
 */
struct aos_condition_handlers {
	/**
	 * \brief Out-Of-Memory handler called when malloc can not allocate the
	 * requested piece of memory.
	 *
	 * This error is not necessary fatal for the current process if it checks
	 * for successfull malloc operation.
	 * 
	 * @param task The struct task_t that that was doing the memory allocation.
	 */
	void (*oom)(struct task_t* task);

	/**
	 * \brief Stack-Alloc-Error. Called when a context-switch could not allocate
	 * memory for a process' stack.
	 *
	 * This error is fatal for the current process but non-fatal for other
	 * processes and operating system operation, unless, of cause, if  all
	 * processes can not be switched out.
	 *
	 * @param task The task that failed to be task-switched.
 */
	void (*sae)(struct task_t* task);
	
};


/**
 * \brief Statistics about CPU-usage
 */
struct cpustat {

	/**
	 * \brief The procentage of time in system-mode.
	 */
	uint8_t system;

	/**
	 * \brief The procentage of time in user-mode.
	 */
	uint8_t user;

	/**
	 * \brief The procentage of time in idle-mode.
	 */
	uint8_t idle;
};

__inline__ uint32_t get_usermode_sp();


/**
 * \brief Internal function used to get the usermode stackpointer
 * @return The stackpointer from usermode
 */
uint32_t get_sp();

/**
 * \brief Create task, and add it to the list of ready processes
 * @param entrypoint The function to thread
 * @param priority The priority it should have. Less is more :)
 * @return Pointer to the created thread. It is not necessary to store it -
 * it can be retrieved as current from the running process.
 */
struct task_t* create_task(funcPtr entrypoint, int8_t priority);

/**
 * \brief Initializes a struct task
 * @param task The struct task to initialize
 * @param entrypoint The function to thread
 * @param priority The priority. Less is more :)
 */
void init_task(struct task_t* task,funcPtr entrypoint, int8_t priority);

/**
 * \brief Do basic initialization.
 */
void aos_basic_init();

/**
 * \brief This function initiates aos operation. NOTE: This function does NOT return.
 * @param timer_refclk The frequency of the timer used as context-switch interrupt.
 */
void aos_context_init(uint32_t timer_refclk, funcPtr idle_func);

void aos_mm_init(void* start, void* end);

/**
 * \brief Block the current process
 */
void block();

/**
 * \brief Unblock a process
 * @param task ointer to a struct task_t to unblock
 */
void unblock(struct task_t* task);

/**
 * \brief Yield the process
 */
void yield();

/**
 * \brief Sleep some milliseconds
 * @param ms Number of milliseconds to sleep
 */
void msleep(uint16_t ms);

/**
 * \brief Sleep some microseconds. Be aware that you can not sleep shorter
 * than the time it takes to do 2 context-switches
 * @param us The microseconds to sleep
 */
void usleep(uint32_t us);

/**
 * \brief Disable context-switching, effectively making the current process
 * owner of the time, except the time it takes to handle interrupts
 */
void disable_cs();

/**
 * \brief Enable context-switching and allow other processes to run
 */
void enable_cs();

/**
 * \brief Get the current system uptime in microseconds.
 * This overflows after 1.1 hours
 * @param time Pointer to store the time
 */
void get_systime(uint32_t* time);

/**
 * \brief Calculate the difference between two uint32_t
 * Overflow compensated
 */
inline static uint32_t uint32diff(uint32_t min, uint32_t max) {
	return min<max ? max-min : max + 0xFFFFFFFF - min;
}

// extern struct task_t* idle_task;
extern struct aos_condition_handlers* condition_handlers;
extern struct task_t* current;
extern uint8_t do_context_switch;

extern struct list_head readyQ;
extern struct list_head usleepQ;

#endif // _KERNEL_H_
