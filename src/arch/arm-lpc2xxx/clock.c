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
#define AOS_KERNEL_MODULE

#include <arm/lpc2119.h>
#include <bits.h>
#include <types.h>
#include <timer.h>
#include <kernel.h>
#include <irq.h>
#include <macros.h>
#include <clock.h>
#include <config.h>

#define TIMER0_IRQ 4

// static int8_t vector_num;

static uint32_t timer_overflows = 0;

void sched_clock(void);

static void clock_isr(void) {
	timer_clock();
	sched_clock();
	system_ticks++;
	T0_IR = BIT0;    // Clear interrupt
}

void init_clock(uint32_t timer_refclk) {
	T0_PR = timer_refclk/1000000 - 1;		/* Scale to 1 us steps */
	T0_PC = 0;													/* Prescale-counter */
	T0_TC = 0x00000000;									/* Counter-value */
	T0_MR0 = T0_TC + (1000000/HZ);	/* Match-Register0 */

	irq_attach(TIMER0_IRQ, clock_isr);
	
	T0_MCR = BIT0|BIT1; /* Interrupt on Math-Register0 */
	T0_TCR = BIT0;	/* Enable timer0 */
}

void enable_clock(void) {
	interrupt_unmask(TIMER0_IRQ);
}

void disable_clock(void) {
	interrupt_mask(TIMER0_IRQ);
}


// uint32_t get_timer_match(void) {
// 	return T0_MR0;
// }


// void set_timer_match(uint32_t matchval) {
// 	T0_MR0 = matchval;
// }


// uint32_t read_timer32() {
// 	static uint32_t last_timer = 0;
// 	register uint32_t new_timer = T0_TC;
// 
//  	if (new_timer < last_timer)
// 		timer_overflows++;
// 
// 	last_timer = new_timer;
// 	
// 	return new_timer;
// }
// 
// uint64_t read_timer64() {
// 	uint32_t timerval = read_timer32();
// 	return timerval + (((uint64_t)timer_overflows)<<32);
// }
