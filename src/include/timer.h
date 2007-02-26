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
#ifndef TIMER_H
#define TIMER_H

#include <aos_module.h>

#include <types.h>

/**
 * \brief Read the current value of the usecond timer.
 * This timer overflows after 1.1 hour
 * @return The timer-value
 */
uint32_t read_timer32(void);

/**
 * \brief Read the current value of the usecond timer.
 * This timer overflows after 500,000 years.
 * @return The timer-value combined with number of overflows
 */
uint64_t read_timer64(void);

#endif
