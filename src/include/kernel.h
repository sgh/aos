
#ifndef _KERNEL_H_
#define _KERNEL_H_

#include <macros.h>
#include <task.h>
#include <types.h>
#include <list.h>
#include <syscalls.h>
#include <string.h>
#include <semaphore.h>

#define SHARED_STACK

/**
 * \brief The size, in bytes, of a saved process-context.
 */
#define CONTEXT_SIZE

extern uint32_t Top_Stack;

__inline__ uint32_t get_usermode_sp();

uint32_t get_sp();

void set_interrupt_mask(uint8_t i);

struct task_t* create_task(funcPtr entrypoint, int8_t priority);
void init_task(struct task_t* task,funcPtr entrypoint, int8_t priority);

void aos_basic_init();
void aos_context_init(uint32_t timer_refclk);
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

extern struct task_t* current;
extern uint8_t do_context_switch;

extern struct list_head readyQ;
extern struct list_head usleepQ;

#endif // _KERNEL_H_
