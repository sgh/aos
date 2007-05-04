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

/* extern symbols */
.extern sched
.extern current
.extern do_task_switch
.extern irq_nest_count


.equ SYSTEM_MODE_NOIRQ, 0xDF @ System-mode with IRQ an FIRQ disabled
.equ SYSTEM_MODE_IRQ, 0xDC @ System-mode with IRQ an FIRQ enabled


/* IRQ prologue- and epilogue macros */
.macro IRQ_prologue
	/* Save r0-r12,LR on User-mode stack */
@ 	STMFD SP!,{r0-r12,LR}
	STMFD SP!,{r0-r3,r12,LR}
	
	/* Increment irq nesting level */
	LDR r0, =irq_nest_count
	LDR r1, [r0]
	ADD r1, r1, #1
	STR r1, [r0]

.endm

.macro IRQ_epilogue
	/* Decrement irq nesting level */
	LDR r0, =irq_nest_count
	LDR r1, [r0]
	SUB r1, r1, #1
	STR r1, [r0]

	/* Restore r0-r12,LR frim User-mode-stack */
@ 	LDMFD SP!,{r0-r12,LR}
	LDMFD SP!,{r0-r3,r12,LR}
	
	/* Substract 4 from return addres - we do this to correctly return in a general matter */
	SUB LR, LR, #4
	B return_from_interrupt
.endm
