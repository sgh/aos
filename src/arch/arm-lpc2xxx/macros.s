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


/* IRQ prologue- and epilogue macros */
.macro IRQ_prologue
	/* Save r0-r12,LR on User-mode stack */
	STMFD SP!,{r0-r3,r12,LR}
	
	/* Increment irq nesting level */
	LDR r0, =irq_nest_count
	LDR r1, [r0]
	ADD r1, r1, #1
	STR r1, [r0]

	/* Store callee cpu-state context */
	MRS r0, SPSR
	STMFD SP!,{r0}

	/* Switch to system-mode */
	MRS r0, CPSR
	BIC r0,r0, #PSR_MODE
	ORR r0, r0, #PSR_MODE_SVC|PSR_NOIRQ /* System-mode and IRQ-disable - since pending interrupts would destry operation */
	MSR CPSR, r0
.endm

.macro IRQ_epilogue

	/* Switch to IRQ-mode */
	MRS r0, CPSR
	BIC r0,r0, #PSR_MODE
	ORR r0, r0, #PSR_MODE_IRQ|PSR_NOIRQ /* System-mode and IRQ-disable - since pending interrupts would destry operation */
	MSR CPSR, r0
		
	/* Retore callee cpu-state context */
	LDMFD SP!,{r0}
	MSR SPSR, r0

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

	/* If nesting count is not zero, just return to whereever the interrupt was asserted */
	MOVS PC, LR
.endm
