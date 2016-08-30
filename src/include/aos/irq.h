/*
	AOS - ARM Operating System
	Copyright (C) 2007  Søren Holm (sgh@sgh.dk)

	License: wxWindows Library Licence, Version 3.1
*/
#ifndef AOS_IRQ_H
#define AOS_IRQ_H

#include <aos/types.h>

#ifdef __cplusplus
	extern "C" {
#endif

#define IRQ_EXCLUSIVE 1

extern volatile int irq_nesting;

#define IRQ_ASSERT() do { if (irq_nesting > 0) \
	assert(__FILE__, __LINE__, __FUNCTION__, \
	"bad irq level"); } while (0)


int irq_attach(int irqnum, void (*isr)(void*), void* arg);

int irq_detach(int irqnum);

void irq_set_prio(uint8_t irqnum, uint8_t prio);

uint8_t irq_handler(int vector);

/**
 * \brief Unmask an irq
 * @param irqnum The irq to unmask
 */
void interrupt_unmask(uint8_t irqnum);

/**
 * \brief Mask an irq
 * @param irqnum The irq to mask
 */
void interrupt_mask(uint8_t irqnum);

void interrupt_init(void);

void irq_lock(void);

void irq_unlock(void);

void irq_init(void);

#ifdef __cplusplus
	}
#endif

#endif
