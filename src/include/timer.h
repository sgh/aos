#ifndef TIMER_H
#define TIMER_H

#include <aos_module.h>

#include <types.h>

/**
 * \brief Read the current value of the usecond timer.
 * This timer overflows after 1.1 hour
 * @return The timer-value
 */
uint32_t read_timer32();

/**
 * \brief Read the current value of the usecond timer.
 * This timer overflows after 500,000 years.
 * @return The timer-value combined with number of overflows
 */
uint64_t read_timer64();

#endif
