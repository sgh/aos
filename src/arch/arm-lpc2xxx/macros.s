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
.extern return_from_interrupt

/* external symbols */
.extern sched
.extern current
.extern do_task_switch
.extern irq_nest_count

.equ PSR_MODE,     0x1F
.equ PSR_MODE_USR, 0x10
.equ PSR_MODE_SVC, 0x13
.equ PSR_MODE_IRQ, 0x12
.equ PSR_MODE_SYS, 0x1F

.equ PSR_NOIRQ, 0xC0
