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
