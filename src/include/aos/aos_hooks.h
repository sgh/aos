/*
	AOS - ARM Operating System
	Copyright (C) 2007  Søren Holm (sgh@sgh.dk)

	License: wxWindows Library Licence, Version 3.1
*/
#ifndef AOS_AOS_HOOKS_H
#define AOS_AOS_HOOKS_H

//#include <aos/task.h>
#include <stdint.h>

#ifdef __cplusplus
	extern "C" {
#endif

/**
 * \brief This struct contains hook-functions to call at different conditions.
 *
 * Theese conditions may not be fatal, but could be.
 * Therefor the hooks should be carefull not to do things that
 * rely on the failing functionality. The hooks are postfixed with 'error',
 * 'fatal' or 'event' to indicate the severeness of the condition.
 *
 * All of these functions are called in privileged mode (system-mode) and
 * one should therefore be carefull to do much more than vital stuff.
 */
struct aos_hooks {
  /**
	 * \brief Out-Of-Memory handler called when malloc can not allocate the
	 * requested piece of memory.
	 *
	 * This error is not necessary fatal for the current process if it checks
	 * for successfull malloc operation.
	 *
	 * @param task The struct task_t that that was doing the memory allocation.
	 */
// 	void (*oom_error)(struct task_t* task);

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
	//void (*stack_alloc_fatal)(struct task_t* task);
	
	/**
	 * \brief This hook is called at every timer-interrupt. Which means every 1 ms.
	 * Watch out what you are doing in this function, since excesive use of
	 * cpu-ressources will kill the system.
	 *
	 * @param time The current time in milliseconds
	 */
	void (*timer_event)(uint32_t time);

	//void (*fatal_event)(const char* str);

	void (*assert_failed)(const char * const exp, const char* const file, int line);
};

/**
 * \brief Syscall to set the struct aos_hooks to use.
 */
void aos_hooks(struct aos_hooks* hooks);

// Only provide this macro for the operating system
#ifdef AOS_KERNEL_MODULE

extern struct aos_hooks* _aos_hooks;

#define AOS_HOOK(hook,args) if (_aos_hooks && _aos_hooks->hook) { _aos_hooks->hook(args); }

// #define AOS_FATAL(str) if (_aos_hooks && _aos_hooks->fatal_event) { _aos_hooks->fatal_event(str); }

// #define AOS_WARNING(str) if (_aos_hooks && _aos_hooks->warning_event) { _aos_hooks->warning_event(str); }

#endif

#ifdef __cplusplus
	}
#endif

#endif


