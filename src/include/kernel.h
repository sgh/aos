#ifndef _KERNEL_H_
#define _KERNEL_H_

#include <aos_module.h>

#include <syscalls.h>
#include <types.h>
#include <task.h>
#include <list.h>

#include <aos.h> /** @todo make this go away */


/**
 * \brief linker-provided placement of the stack
 * @todo it could be nice to have this detected automatically.
 */
extern uint32_t Top_Stack;

/**
 * \brief Internal function to get the usermode stackpointer.
 * @return The stackpointer from usermode
 */
__inline__ uint32_t get_usermode_sp();


/**
 * \brief Internal function used to get the current stack-pointer.
 * @return The current stackpointer
 */
__inline__ uint32_t get_sp();

/**
 * \brief The idle task
 */
extern struct task_t* idle_task;

/**
 * \brief The aos hooks
 */
extern struct aos_hooks* _aos_hooks;

/**
 * @todo write this
 */
extern struct aos_status _aos_status;

/**
 * \brief Shall we do proccess-shift.
 */
extern uint8_t do_context_switch;


/**
 * \brief This queue hold processes that sleep.
 */
extern struct list_head usleepQ;


/**
 * \brief Wakeup the process and insert it in the readyQ
 * @param task The task to wakeup.
 */
void process_wakeup(struct task_t* task);


#endif // _KERNEL_H_
