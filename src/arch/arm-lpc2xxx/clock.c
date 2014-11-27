/*
	AOS - ARM Operating System
	Copyright (C) 2007  Søren Holm (sgh@sgh.dk)

	License: wxWindows Library Licence, Version 3.1
*/
#define AOS_KERNEL_MODULE

#include <arm/lpc2119.h>
#include <aos/bits.h>
#include <aos/types.h>
#include <aos/timer.h>
#include <aos/kernel.h>
#include <aos/irq.h>
#include <aos/macros.h>
#include <aos/clock.h>
#include <aos/config.h>
#include <aos/interrupt.h>

#define TIMER0_IRQ 4


static void clock_isr(UNUSED void* arg) {
	system_ticks++;

	timer_clock();
	sched_clock();

	AOS_HOOK(timer_event,ticks2ms(system_ticks));

	T0_IR = BIT0;    // Clear interrupt
}

//mutex_t clocklock;

void init_clock(uint32_t timer_refclk) {
	T0_PR = timer_refclk/1000000 - 1;		/* Scale to 1 us steps */
	T0_PC = 0;													/* Prescale-counter */
	T0_TC = 0x00000000;									/* Counter-value */
	T0_MR0 = T0_TC + (1000000/HZ);	/* Match-Register0 */

	irq_attach(TIMER0_IRQ, clock_isr, NULL);
	
	T0_MCR = BIT0|BIT1; /* Interrupt on Math-Register0 */
	T0_TCR = BIT0;	/* Enable timer0 */
}

void enable_clock(void) {
	interrupt_unmask(TIMER0_IRQ);
}

void disable_clock(void) {
	interrupt_mask(TIMER0_IRQ);
}

uint32_t get_clock(void) {
	return T0_TC;
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
