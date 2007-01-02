#ifndef AOS_H
#define AOS_H

#include <semaphore.h>
#include <mutex.h>
#include <list.h>
#include <macros.h>
#include <task.h>
#include <spinlock.h>
#include <types.h>
#include <atomic.h>
#include <mm.h>
#include <aos_hooks.h>
#include <aos_status.h>

/**
 * \brief The readyQ. This is hwere processes lie when they are ready to run.
 */
extern struct list_head readyQ;

/**
 * \brief Current running process
 */
extern struct task_t* current;

/**
 * \brief Create task, and add it to the list of ready processes
 * @param entrypoint The function to thread
 * @param arg A void* argument for the process. Enables multiple equal
 * processes with different data.
 * @param priority The priority it should have. Less is more :)
 * @return Pointer to the created thread. It is not necessary to store it -
 * it can be retrieved as current from the running process.
 */
struct task_t* create_task(funcPtr entrypoint, void* arg, int8_t priority);

/**
 * \brief Do basic initialization.
 */
void aos_basic_init();

/**
 * \brief This function initiates aos operation. NOTE: This function does NOT return.
 * @param timer_refclk The frequency of the timer used as context-switch interrupt.
 */
void aos_context_init(uint32_t timer_refclk, funcPtr idle_func);

/**
 * \brief Initialize dynamic memory.
 * @param start Pointer to first address usable by malloc.
 * @param end Pointer to last address usable by malloc.
 */
void aos_mm_init(void* start, void* end);

/**
 * \brief Block the current process. Remember to have placed the process in a queue.
 * Forgetting to do so would block the process forever.
 */
void block();

/**
 * \brief Unblock a process
 * @param task ointer to a struct task_t to unblock
 */
void unblock(struct task_t* task);

/**
 * \brief Yield the process.
 */
void yield();

/**
 * \brief Sleep some milliseconds
 * @param ms Number of milliseconds to sleep
 */
void msleep(uint16_t ms);

/**
 * \brief Sleep some microseconds.
 * Be aware that nomatter what value you supply you can not sleep shorter
 * than the time it takes to do 2 context-switches.
 * @param us The microseconds to sleep
 */
void usleep(uint32_t us);

/**
 * \brief Disable context-switching, effectively making the current process
 * owner of the time, except the time it takes to handle interrupts.
 */
void disable_cs();

/**
 * \brief Enable context-switching and allow other processes to run
 */
void enable_cs();

/**
 * \brief Get the current system uptime in microseconds.
 * This overflows after 1.1 hours.
 * @param time Pointer to store the time.
 */
void get_sysutime(uint32_t* time);

/**
 * \brief Get the current system uptime in milliseconds
 * This overflows after 49 days.
 * @param time Pointer to store the time.
 */
void get_sysmtime(uint32_t* time);


/**
 * \brief Syscall to unmask all interrupts
 */
void enable_irqs();

/**
 * \brief Syscall to unmask all interrupts
 */
void disable_irqs();



#endif
