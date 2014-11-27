/*
	AOS - ARM Operating System
	Copyright (C) 2007  Søren Holm (sgh@sgh.dk)

	License: wxWindows Library Licence, Version 3.1
*/
#ifndef AOS_TIMER_H
#define AOS_TIMER_H

#define AOS_KERNEL_MODULE

#include <aos/aos_module.h>
#include <aos/list.h>
#include <aos/types.h>

struct timer {
	uint8_t type;
	uint32_t expire;
	void (*func)(void*);
	void* arg;
	struct list_head node;
};

#define TMR_STOP      1
#define TMR_PERIODIC  2
#define TMR_TIMEOUT   4

extern volatile uint32_t system_ticks;

/**
 * \brief Read the current value of the usecond timer.
 * This timer overflows after 1.1 hour
 * @return The timer-value
 */
// uint32_t read_timer32(void);

/**
 * \brief Read the current value of the usecond timer.
 * This timer overflows after 500,000 years.
 * @return The timer-value combined with number of overflows
 */
// uint64_t read_timer64(void);

void timer_stop(struct timer* tmr);

void timer_timeout(struct timer* tmr, void (*func)(void*), void* arg, uint32_t expire);

#endif
