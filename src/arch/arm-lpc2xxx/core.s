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

/* public symbols */
.global aos_swi_entry
.global aos_irq_entry
.global sched_unlock
.global sched_lock
.global syscall_ret

/* Public interrupt-handler symbols */
.global timer_interrupt

.include "arch/arm-lpc2xxx/macros.s"

/*
	Software-interrupt handler:
		We get a seperate SPSR, R13(SP) and R14(LR) because
		we are in supervisor-mode
		
		Leave r0-r5 alone because they hold the arguments
		and r0 the return value
*/
aos_swi_entry:
	/* Save registers on SWI-mode stack */
	SUB SP, SP, #(19*4)
	STMIA SP,{r0-r14}^
	NOP
	ADD r5, SP, #(19*4)

	STR	r5, [SP, #(15*4)] /* Push svc_sp */
	STR	lr, [SP, #(17*4)] /* Push pc */
	MRS	r5, SPSR
	STR	r5, [SP, #(18*4)] /* Push cpsr */

	/* Fetch SPRS application CPSR */
	MRS r7, SPSR

	/* Read LR to see if the SWI-instruction was in ARM-, or THUMB-mode */
	AND r7, r7, #0x20
	CMP r7, #0
	
	BEQ _get_swinum_arm

	/* Get number embedded in SWI-instruction, either THUMB or ARM */
_get_swinum_thumb:
	/* Load SWI-number from THUMB SWI-instruction */
	LDRB r6, [LR, #-2]
	BIC r6, r6 ,#0xFFFFFF00
	B _after_get_swinum

_get_swinum_arm:
	LDR r6, [LR, #-4]
	BIC r6, r6 ,#0xFF000000

_after_get_swinum:

	/* syscall offset */
	MOV r6, r6, LSL #2 @ TODO Boundcheck this value

	/* Calculate offset */
	LDR r7, =sys_call_table
	LDR r7, [r7, r6]

	/* Set LR and call routine */
	MOV LR, PC
	BX r7

	STR r0, [SP] @ Store in r0
syscall_ret:
	/* Restore registers from SWI-mode stack */
	MOV r5, SP
	LDR	SP, [r5, #(4*15)] /* Restore svc_sp */
	LDR	LR, [r5, #(4*17)] /* Restore pc (lr) */
	LDR	r1, [r5, #(4*18)] /* Restore cpsr */
	MSR	SPSR_all, r1

	LDMFD r5, {r0-r14}^   /* Restore user mode registers */
	NOP                   /* Instruction gap for ldm^ */
	MOVS PC, LR          /* Exit, with restoring cpsr */


.equ    I_Bit,          0x80    /* when I bit is set, IRQ is disabled */
.equ    F_Bit,          0x40    /* when F bit is set, FIQ is disabled */


/**
 * Swtiches context r0:old r1:new
 */
.global switch_context
switch_context:
	stmia	r0, {r4-r11, sp, lr}	/* Save previous register context */
	ldmia	r1, {r4-r11, sp, pc}	/* Restore next register context */
	

/*
	Common-interrupt entry
*/
aos_irq_entry:
	@ First store the registers that we destroy on the IRQ stack without SP writeback
	STMFD SP, {r0-r4}
	SUB	r2, LR, #4		/* r2: PC */
	MRS	r3, SPSR		/* r3: CPSR */
	SUB r4, SP, #(5*4) @ Save address of start of r0-r4 in r4

	@ Next Change to Supervisor-mode using r0 as CPSR storage
	MRS r0, CPSR
	BIC r0, r0, #PSR_MODE
	ORR r0, r0, #PSR_MODE_SVC
	MSR CPSR_c, r0

	@ Now copy svc_LR and svc_SP to two other general registers
	MOV r0, SP
	MOV r1, LR

	@ Push the four register on stack with writeback
	STMFD SP!, {r0-r3} @ SP_svc, LR_svc,, PC_app, CPSR_app

	@ And restore the work registers we saved a little while ago
	LDMFD r4, {r0-r4}

	sub	sp, sp, #(4*15)
	STMIA SP,{r0-r14}^ @ Store all 13 general registers
	NOP

	LDR r5, =irq_nest_count @ r5: &nesting count
	LDR r6, [r5] @ r6: nesting count
	ADD r0, r6, #1
	STR r0, [r5]

	@ Check if nesting level is 0
	CMP r6, #0
	BLEQ sched_lock

	BL interrupt_handler

	STR r6, [r5] @ Store old nesting count

	@ Check if nesting level is 0
	CMP r6, #0
	BNE nested_irq

	BL sched_unlock

nested_irq:
	@ Restore Supervisor-mode registers
	MOV r0, SP @ Save SP in r0 to enable restore in IRQ-mode
	LDR SP, [r0, #(15*4)] @ SP
	LDR LR, [r0, #(16*4)] @ LR

	/* Switch to IRQ-mode using r0 */
	MRS r1, CPSR
	BIC r1, r1, #PSR_MODE
	ORR r1, r1, #PSR_MODE_IRQ/*|PSR_NOIRQ*/ /* System-mode and IRQ-disable - since pending interrupts would destry operation */
	MSR CPSR_c, r1

	@ Reload IRQ specific registers
	LDR LR, [r0, #(17*4)] @ LR (PC)
	LDR r1, [r0, #(18*4)] @ SPSR
	MSR SPSR_all, r1

	LDMFD r0, {r0-r14}^
	NOP
	
	MOVS PC, LR @ Else just return to previous mode
