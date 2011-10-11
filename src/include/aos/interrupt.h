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
#ifndef AOS_INTERRUPT_H
#define AOS_INTERRUPT_H

void aos_irq_entry(void);

static inline void interrupt_enable(void) {
#ifndef __lint__
	uint32_t val;
	
	__asm__ __volatile__ (
		"mrs %0, cpsr\n\t"
		"bic %0, %0, #0xc0\n\t"		/* Enable IRQ & FIQ */
		"msr cpsr_c, %0\n\t"
	:"=&r" (val)
	:
	: "memory");
#endif
}


static inline void interrupt_disable(void) {
#ifndef __lint__
	uint32_t val;
	
	__asm__ __volatile__ (
		"mrs %0, cpsr\n\t"
		"orr %0, %0, #0xC0\n\t"		/* Disable IRQ & FIQ */
		"msr cpsr_c, %0\n\t"
	:"=&r" (val)
	:
	: "memory");
#endif
}

static inline void interrupt_save(uint32_t *sts) {
	uint32_t val;

	__asm__ __volatile__ (
		"mrs %0, cpsr\n\t"
	:"=&r" (val)
	:
	:"memory");

	*sts = (uint32_t)val;
}

static inline void interrupt_restore(uint32_t sts) {
	__asm__ __volatile__ (
		"msr cpsr_c, %0\n\t"
	:
	:"r" (sts)
	:"memory");

}

#endif
