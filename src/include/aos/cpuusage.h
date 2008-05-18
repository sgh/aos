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
#ifndef CPUUSAGE_H
#define CPUUSAGE_H

#include <types.h>

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

#endif
