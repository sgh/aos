/*
		AOS - ARM Operating System
		Copyright (C) 2007  Søren Holm (sgh@sgh.dk)

		This library is free software; you can redistribute it and/or
		modify it under the terms of the GNU Lesser General Public
		License as published by the Free Software Foundation; either
		version 2.1 of the License, or (at your option) any later version.

		This library is distributed in the hope that it will be useful,
		but WITHOUT ANY WARRANTY; without even the implied warranty of
		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
		Lesser General Public License for more details.

		You should have received a copy of the GNU Lesser General Public
		License along with this library; if not, write to the Free Software
		Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
		*/
#ifndef TIMER_INTERRUPT_H
#define TIMER_INTERRUPT_H

#include <aos_module.h>

#include <types.h>

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

/**
 * \brief Clear the interrupt. Only used at the end of the interrupt-handler
 */
void clear_timer_interrupt(void);

/**
 * \brief Set the timer-value at which an interrupt should next be taken.
 * @param matchval The targe-value of the timer
 */
void set_timer_match(uint32_t matchval);

/**
 * \brief Get the timer-value at which an interrupt should next be taken.
 * @return The match-value.
 */
uint32_t get_timer_match(void);

/**
 * \brief The timer-interrupt. Executed 1000 times pr. second.
 */
void timer_clock(void);

/**
 * \brief Timer value at last interrupt
 */
extern uint32_t last_context_time;

#endif

