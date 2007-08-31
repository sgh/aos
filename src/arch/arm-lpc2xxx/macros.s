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


.macro IRQ_START
	@ First store the registers that we destroy on the IRQ stack without SP writeback
	STMFD SP, {r0-r4}
	SUB r4, SP, #(5*4) @ Save address of start of r0-r4 in r4
	
	@ To do nested interrupts we need to first get the irq_LR and irq_SPSR into some general registers
	SUB r2, LR, #4
	MRS r3, SPSR

	@ Next Change to System-mode using r0 as CPSR storage
	MRS r0, CPSR
	BIC r0, r0, #PSR_MODE
	ORR r0, r0, #PSR_MODE_SYS|PSR_NOIRQ
	MSR CPSR_c, r0

	@ Now copy svc_LR and svc_SP to two other general registers
	MOV r0, LR
	MOV r1, SP

	@ Push the four register on stack with writeback
	STMFD SP!, {r0-r3} @ LR_svc, SP_svc, PC_app, CPSR_app

	@ And restore the work registers we saved a little while ago
	LDMFD r4, {r0-r4}

	/*
		We should now have tranfered to System-mode and the stacks look like this
		
		IRQ-stack is empty.
	
		System-mode-stack contains this.

		+----------------------------+
		| CPSR from interrupted mode |
	  +----------------------------+
		| PC from interrupted mode   |
	  +----------------------------+
	  | SVC Stack pointer          |
	  +----------------------------+
	  | SVC Link Register          |
	  +----------------------------+
	*/
.endm


.macro IRQ_END
	@ Restore System-mode registers
	LDR LR, [r0, #(-4*4)] @ LR
	LDR SP, [r0, #(-3*4)] @ SP

	/* Switch to IRQ-mode using r0 */
	MRS r1, CPSR
	BIC r1,r1, #PSR_MODE
	ORR r1, r1, #PSR_MODE_IRQ/*|PSR_NOIRQ*/ /* System-mode and IRQ-disable - since pending interrupts would destry operation */
	MSR CPSR_c, r1

	@ Reload IRQ specific registers
	LDR LR, [r0, #(-2*4)] @ LR
	LDR r1, [r0, #(-1*4)] @ SPSR
	MSR SPSR, r1
.endm


.macro SWI_START
	@ First store the registers that we destroy on the SVC stack without SP writeback
	STMFD SP, {r5-r9}
	SUB r9, SP, #(5*4) @ Save address of start of r0-r4 in r4
	
	@ To do nested interrupts we need to first get the svc_LR and svc_SPSR into some general registers
	SUB r7, LR, #4
	MRS r8, SPSR

	@ Next Change to System-mode using r0 as CPSR storage
	MRS r5, CPSR
	BIC r5, r5, #PSR_MODE
	ORR r5, r5, #PSR_MODE_SYS|PSR_NOIRQ
	MSR CPSR_c, r5

	@ Now copy sts_LR and sys_SP to two other general registers
	MOV r5, LR
	MOV r6, SP

	@ Push the four register on stack with writeback
	STMFD SP!, {r5-r8} @ LR_svc, SP_svc, PC_app, CPSR_app

	@ And restore the work registers we saved a little while ago
	LDMFD r9, {r5-r9}

	/*
		We should now have tranfered to System-mode and the stacks look like this
		
		SVC-stack is empty.
	
		System-mode-stack contains this.

		+----------------------------+
		| CPSR from interrupted mode |
	  +----------------------------+
		| PC from interrupted mode   |
	  +----------------------------+
	  | SVC Stack pointer          |
	  +----------------------------+
	  | SVC Link Register          |
	  +----------------------------+
	*/
.endm


.macro SWI_END
	@ Restore System-mode registers
	LDR LR, [r5, #(4*-4)] @ LR
	LDR SP, [r5, #(3*-4)] @ SP

	/* Switch to SVC-mode using r0 */
	MRS r6, CPSR
	BIC r6,r6, #PSR_MODE
	ORR r6, r6, #PSR_MODE_SVC|PSR_NOIRQ /* System-mode and IRQ-disable - since pending interrupts would destry operation */
	MSR CPSR_c, r6

	@ Reload IRQ specific registers
	LDR LR, [r5, #(2*-4)] @ LR
	LDR r6, [r5, #(1*-4)] @ SPSR
	MSR SPSR, r6
.endm
