/*
	AOS - ARM Operating System
	Copyright (C) 2007  Søren Holm (sgh@sgh.dk)

	License: wxWindows Library Licence, Version 3.1
*/
#ifndef AOS_CLOCK_H
#define AOS_CLOCK_H

#include <aos/aos_module.h>

#include <aos/types.h>

/**
 * \brief Initialize a 1000Hz timer_interrupt
 * @param handler Pointer to the funtion to handle the interrupt
 * @param timer_refclk The timer-frequency. This is used to calculate
 * the currect timer prescaler
 */
void init_clock(uint32_t timer_refclk);

/**
 * \brief Enable the timer-interrupt and start the timer
 */
void enable_clock(void);

/**
 * \brief Disable the timer-interrupt and stop the timer
 */
void disable_clock(void);

uint32_t get_clock(void);

/**
 * \brief Clear the interrupt. Only used at the end of the interrupt-handler
 */
// void clear_timer_interrupt(void);

/**
 * \brief Set the timer-value at which an interrupt should next be taken.
 * @param matchval The targe-value of the timer
 */
// void set_timer_match(uint32_t matchval);

/**
 * \brief Get the timer-value at which an interrupt should next be taken.
 * @return The match-value.
 */
// uint32_t get_timer_match(void);

/**
 * \brief The timer-interrupt. Executed 1000 times pr. second.
 */
void timer_clock(void);

/**
 * \brief Timer value at last interrupt
 */
// extern uint32_t last_context_time;

#endif

