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
#include <types.h>
#include <bits.h>
#include <irq.h>
#include <macros.h>
#include <interrupt.h>

void interrupt_handler(void);


void interrupt_unmask(uint8_t irqnum) {
	if (irqnum>31)
		return;
	VICIntEnable |= BIT0<<irqnum; /* Enable Interrrupt */
}


void interrupt_mask(uint8_t irqnum) {
	if (irqnum>31)
		return;
	VICIntEnable &= ~(BIT0<<irqnum); /* Enable Interrrupt */
}

void interrupt_init(void) {
	VICDefVectAddr = (uint32_t)aos_irq_entry;
}


static void interrupt_dispatch(int vector) {
	irq_handler(vector);
	VICVectAddr = 0; // Update priority hardware
}


void interrupt_handler(void) {
	int vector;
	int bits;

	bits = VICIRQStatus;

retry:
		for (vector = 0; vector<32; vector++) {
	if (bits & (1<<vector))
		break;
		}

		interrupt_dispatch(vector);

		bits = VICIRQStatus;

		if (bits)
			goto retry;
}
