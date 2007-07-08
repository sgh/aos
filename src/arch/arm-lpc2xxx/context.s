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


/* Public interrupt-handler symbols */
.global timer_interrupt
;.global uart0_interrupt

.include "arch/arm-lpc2xxx/macros.s"

/*
	Software-interrupt handler:
		We get a seperate SPSR, R13(SP) and R14(LR) because
		we are in supervisor-mode
		
		Leave r0-r5 alone because they hold the arguments
		and r0 the return value
*/
aos_swi_entry:
	STMFD SP!,{r0-r4,LR}
	BL irqs_are_disabled
	LDMFD SP!,{r0-r4,LR}
	
	/* Save registers on SWI-mode stack */
	STMFD SP!,{r6-r7, LR}

	/* Read LR to see if the SWI-instruction was in ARM-, or THUMB-mode */
	MRS r7, SPSR
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
	MOV r6, r6, LSL #2

	/* Calculate offset */
	LDR r7, =sys_call_table
	LDR r7, [r7, r6]
	
	/* Set LR and call routine */
	MOV LR, PC
	BX r7
	
	/* Restore registers from SWI-mode stack */
	LDMFD SP!,{r6-r7, LR}
	
	/* Call common interrupt escape code */
	B return_from_interrupt

/*
	Calculation of context store:
		This routine returns the address where we can store
		registers from current context.

		r0 and r1 is considered to be scratch
*/
_get_current_context_store:
	/* Get address of current process-context */
	LDR r0, =context_offset
	LDR r0, [r0]
	LDR r1, =current
	LDR r1, [r1]	
	LDR r0, [r0, r1]
	MOV PC, LR


/*
	Common-interrupt entry
*/
aos_irq_entry:

	@ First store the registers that we destroy on the IRQ stack without SP writeback
	STMFD SP, {r0-r4}
	SUB r4, SP, #(5*4) @ Save address of start of r0-r4 in r4
	
	@ To do nested interrupts we need to first get the irq_LR and irq_SPSR into some general registers
	SUB r2, LR, #4
	MRS r3, SPSR

	@ Next Change to SVC-mode using r0 as CPSR storage
	MRS r0, CPSR
	BIC r0, r0, #PSR_MODE
	ORR r0, r0, #PSR_MODE_SVC
	MSR CPSR_c, r0

	@ Now copy svc_LR and svc_SP to two other general registers
	MOV r0, LR
	MOV r1, SP

	@ Push the four register on stack with writeback
	STMFD SP!, {r0-r3} @ LR_svc, SP_svc, PC_app, CPSR_app

	@ And restore the work registers we saved a little while ago
	LDMFD r4, {r0-r4}

	/*
		We should now have tranfered to svc and the stacks look like this
		
		IRQ-stack is empty.
	
		SVC-stack contains this.

		+----------------------------+
		| CPSR from interrupted mode |
	  +----------------------------+
		| PC from interrupted mode   |
	  +----------------------------+
	  | SVC Stack pointer          |
	  +----------------------------+
	  | SV Link Register           |
	  +----------------------------+
	*/

	STMFD SP!,{r0-r12} @ Store all 13 general registers

	LDR r5, =irq_nest_count @ r5: &nesting count
	LDR r6, [r5] @ r6: nesting count
	ADD r0, r6, #1
	STR r0, [r5]

	BL interrupt_handler

	STR r6, [r5] @ Store old nesting count

	MOV r0, SP @ Save SP in r0 to enable restore in IRQ-mode

	ADD SP, SP, #(17*4) @ Move over the 13 general registers and the 4 registers

	@ Restore LR registers
	LDR LR, [r0, #(13*4)] @ LR
	LDR SP, [r0, #(14*4)] @ SP

	/* Switch to IRQ-mode using r0 */
	MRS r1, CPSR
	BIC r1,r1, #PSR_MODE
	ORR r1, r1, #PSR_MODE_IRQ|PSR_NOIRQ /* System-mode and IRQ-disable - since pending interrupts would destry operation */
	MSR CPSR_c, r1

	@ Reload IRQ specific registers
	LDR LR, [r0, #(15*4)] @ LR
	LDR r1, [r0, #(16*4)] @ SPSR
	MSR SPSR, r1

	LDR r1, =irq_nest_count
	LDR r1, [r1]
	CMP r1, #0

	LDMFD r0, {r0-r12}
	
	BEQ return_from_interrupt @ Return via. common returncode if on top-level irq
	MOVS PC, LR @ Else just return to previous mode


/*
	Common interrupt escape routine:
		Returns correctly from SWIs and IRQs.
		Does context-switching if do_context_switch != 0
*/
return_from_interrupt:
	/* Save r0 on stack - we restore it later in context save and during no_task_switch */
	STMFD SP!,{r0}

	/* Are we going to switch tasks */
	LDR r0, =do_context_switch
	LDRB r0, [r0]
	CMP r0, #0
	BEQ _no_task_switch

	/* If allow_context_switch is 0, then we must not do context_switch this time */
	LDR r0, =allow_context_switch
	LDRB r0, [r0]
	CMP r0, #0
	BEQ _no_task_switch

@======================================================================================

	/* Set do_context_switch to 0 */
	STMFD SP!,{r1}	@ Store r1
	LDR r0, =do_context_switch
	MOV r1, #0
	STRB r1, [r0]
	LDMFD SP!,{r1} @ Restore r1

	/* If current is 0, then we must not try so save current context */
	LDR r0, =current
	LDR r0, [r0]
	CMP r0, #0
	BEQ _after_task_save
	
	/* Save r1,LR on stack since the next routine-call will destroy them */
	STMFD SP!,{r1,LR}
	
	/*
		Call routine to calculate location of context-store.
		We MUST save LR here. It is the return-address in User-mode
	*/
	BL _get_current_context_store
	
	/* Restore r1,LR again */
	LDMFD SP!,{r1,LR}


/*
	In the following we do the actual context-save and -restore.
	The memory in which the 17 registers are saved looks like this (from cpu.h)

	PC, SP, LR, SPSR, r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12
*/
	
	/*
		Store LR - the addres at which we must return
		to when this process is to run again.
	*/
	STR LR, [r0]

	/* Store r1-r12 @ r0 + 20 */
	ADD r0, r0, #20
	STMIA r0,{r1-r12}
	SUB r0, r0, #20

	/* Store SPSR process-cpu-flags @ r0 + 16 */
	MRS r1, SPSR
	STR r1, [r0, #3*4]

	/*
		Load the value of r0, which we previously saved on stack.
		SP is not affected during this
	*/
	LDR r1, [SP]

	STR r1, [r0, #4*4] @ Save r0-value

	/* Save current mode in r10 */
	MRS r10, CPSR

	/* Switch to system-mode */
	MOV r9, r10
	BIC r9,r9, #0xFF
	ORR r9, r9, #PSR_MODE_SYS|PSR_NOIRQ /* System-mode and IRQ-disable - since pending interrupts would destroy operation */
	MSR CPSR, r9

	/* Move SP to r2 - so we can access it from interrupt-mode */
	MOV r2, SP

	/* Move LR to r3- so we can access it from interrupt-mode */
	MOV r3, LR

	/* Switch to previously saved mode */
	MSR CPSR, r10

	/* Save r2-r3 (SP, LR) at [r0 + 4] */
	ADD r0, r0, #4
	STMIA r0, {r2-r3}

_after_task_save:
	/* From this point on we have all registers to our selves */

	/** @todo Make sched() interruptible */
	BL sched

	/* Here the process restore starts. WATCH THE REGISTERS */
	BL _get_current_context_store

	LDR LR, [r0]       @ Load PC

	LDR r1, [r0, #1*4] @ Load SP
	LDR r2, [r0, #2*4] @ Load LR

	/* Save current mode in r9 */
	MRS r10, CPSR

	/* Switch to system-mode */
	MOV r9, r10
	BIC r9,r9, #PSR_MODE
	ORR r9, r9, #PSR_MODE_SYS|PSR_NOIRQ /* System-mode and IRQ-disable - since pending interrupts would destry operation */
	MSR CPSR, r9

	/* Set SP and LR */
	MOV SP, r1
	MOV LR, r2

	/* Switch to previously saved mode */
	MSR CPSR, r10
	
	LDR r1, [r0, #4*4]  @ Load r0-value

	/* Save value on stack, which will be loaded later. This was stored earlier also */
	STR r1, [SP]

	/* Restore SPSR process-cpu-flags @ r0 - 4 */
	LDR r1, [r0, #3*4]
	MSR SPSR, r1

	ADD r0, r0, #5*4
	LDMIA r0,{r1-r12}   @ Load r1-r12 @ [r0 + 5*4]

@======================================================================================
_no_task_switch:

return_from_irq:
	LDMFD SP!,{r0} @ Load r0 from stack

	STMFD SP!,{r0-r4,LR}
	BL irqs_are_enabled
	LDMFD SP!,{r0-r4,LR}
	
	MOVS PC, LR

