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
void init_timer_interrupt(funcPtr handler, uint32_t timer_refclk);

/**
 * \brief Enable the timer-interrupt and start the timer
 */
void enable_timer_interrupt();

/**
 * \brief Disable the timer-interrupt and stop the timer
 */
void disable_timer_interrupt();

/**
 * \brief Clear the interrupt. Only used at the end of the interrupt-handler
 */
void clear_timer_interrupt();

/**
 * \brief Set the timer-value at which an interrupt should next be taken.
 * @param matchval The targe-value of the timer
 */
void set_timer_match(uint32_t matchval);

/**
 * \brief Get the timer-value at which an interrupt should next be taken.
 * @return The match-value.
 */
uint32_t get_timer_match();

/**
 * \brief The timer-interrupt. Executed 1000 times pr. second.
 */
void timer_interrupt();

/**
 * \brief Timer value at last interrupt
 */
extern uint32_t last_interrupt_time;

#endif

