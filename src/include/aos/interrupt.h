/*
	AOS - ARM Operating System
	Copyright (C) 2007  Søren Holm (sgh@sgh.dk)

	License: wxWindows Library Licence, Version 3.1
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
