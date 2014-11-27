/*
	AOS - ARM Operating System
	Copyright (C) 2007  Søren Holm (sgh@sgh.dk)

	License: wxWindows Library Licence, Version 3.1
*/
#ifndef AOS_CPUUSAGE_H
#define AOS_CPUUSAGE_H

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
