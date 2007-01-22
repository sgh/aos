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

#include <aos_module.h>

#include <types.h>

/**
 * \brief Request a vector for a interrupt
 * @param address The function-address to handle the interrupt
 * @param inrqnum The irq we want to use
 * @return A vector-number to use in vector_enable/disable-calls
 */
uint8_t request_vector(uint32_t address, uint8_t inrqnum);

/**
 * \brief Enable the interrupt-vector
 * @param vectornum The vector-num to enable
 */
void vector_enable(uint8_t vectornum);

/**
 * \brief Disable the interrupt-vector
 * @param vectornum The vector-num to disable
 */
void vector_disable(uint8_t vectornum);

/**
 * \brief Enable an irq
 * @param irqnum The irq to enable
 */
void irq_enable(uint8_t irqnum);

/**
 * \brief Disable an irq
 * @param irqnum The irq-to disable
 */
void irq_disable(uint8_t irqnum);

#endif
