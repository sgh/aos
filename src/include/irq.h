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
#ifndef IRQ_H
#define IRQ_H

#include <types.h>

extern volatile int irq_nesting;

#define IRQ_ASSERT() do { if (irq_nesting > 0) \
	assert(__FILE__, __LINE__, __FUNCTION__, \
	"bad irq level"); } while (0)


int irq_attach(int irqnum, void (*isr)(void*), void* arg);

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



#endif
