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
.equ IRQ_MODE_NOIRQ, 0x92 @ System-mode with IRQ an FIRQ disabled


/* IRQ prologue- and epilogue macros */
.macro IRQ_prologue
	/* Save r0-r12,LR on User-mode stack */
	STMFD SP!,{r0-r3,r12,LR}
	
	/* Increment irq nesting level */
	LDR r0, =irq_nest_count
	LDR r1, [r0]
	ADD r1, r1, #1
	STR r1, [r0]

	/* Store IRQ context */
	MRS r0, SPSR
	STMFD SP!,{r0}

	/* Switch to system-mode */
@ 	MSR CPSR_c, SYSTEM_MODE_NOIRQ
.endm

.macro IRQ_epilogue

	/* Switch to IRQ-mode */
@ 	MSR CPSR_c, IRQ_MODE_NOIRQ
		
	/* Restore IRQ context */
	LDMFD SP!,{r0}
	MSR SPSR_cf, r0

	/* Decrement irq nesting level */
	LDR r0, =irq_nest_count
	LDR r1, [r0]
	SUB r1, r1, #1
	STR r1, [r0]

	/* Test if irq nesting count is zero */
	LDR r0, =irq_nest_count
	LDR r0, [r0]
	CMP r0, #0

	/* Restore r0-r12,LR from User-mode-stack */
	LDMFD SP!,{r0-r3,r12,LR}
	
	/* Substract 4 from return addres - we do this to correctly return in a general matter */
	SUB LR, LR, #4

	/* Return via. common returncode if on top-level irq */
	BEQ return_from_interrupt

	/* If nesting count is not zero, just return */
	MOVS PC, LR
.endm
