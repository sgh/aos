
/* public symbols */
.global SWI_Handler
.global get_usermode_sp
.global get_sp
.global get_usermode_sp


/* Public interrupt-handler symbols */
.global timer_interrupt
;.global uart0_interrupt

.include "kernel/macros.s"

get_sp:
	MOV r0, SP
	MOV PC, LR

get_usermode_sp:
	/* Save current mode in r10 */
	STMFD SP!, {r9-r10}
	MRS r10, CPSR
	
	/* Switch to system-mode */
	MOV r9, r10
	BIC r9,r9,#0xFF
	ORR r9, r9, #SYSTEM_MODE_NOIRQ /* System-mode and IRQ-disable - since pending interrupts would destroy operation */
	MSR CPSR, r9
	
	MOV r0, SP /* Save stackpointer */
	
	/* Switch to previously saved mode */
	MOV r9, r10
	MSR CPSR, r9
	
	LDMFD SP!, {r9-r10}
	BX LR

/*
	Software-interrupt handler:
		We get a seperate SPSR, R13(SP) and R14(LR) because
		we are in supervisor-mode
		
		Leave r0-r5 alone because they hold the arguments
		and r0 the return value
*/
SWI_Handler:
	/* Save registers on SWI-mode stack */
	STMFD SP!,{r6-r12, LR}

	/* Read LR to see if the SWI-instruction was in ARM-, or THUMB-mode */
	MRS r7, SPSR
	AND r6, r7, #0x20
	CMP r6, #0
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
	ADD r7, r7, r6
	LDR r7, [r7]
	
	/* Set LR and call routine */
	MOV LR, PC
	BX r7
	
	/* Restore registers from SWI-mode stack */
	LDMFD SP!,{r6-r12, LR}
	
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
	ADD r0, r0, r1
	LDR r0, [r0]
	MOV PC, LR

/*
	UART0-interrupt handler
*/
@uart0_interrupt:
@	IRQ_prologue
@
@	/* Call C-interrupt-routine */
@	BL uart0_interrupt_routine
@	
@	IRQ_epilogue


/*
	Timer-interrupt handler:
		Called as a result of interrupts from Timer0
*/
timer_interrupt:
	IRQ_prologue

	/* Call C-interrupt-routine */
	LDR r5, =timer_interrupt_routine
	MOV LR, PC
	BX r5

	IRQ_epilogue


/*
	Common interrupt escape routine:
		Returns correctly from SWIs and IRQs.
		Does context-switching if do_context_switch != 0
*/
return_from_interrupt:
	/* Save r0 on stack - we restore it later */
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
	
	/* Save r1 on stack since the next routine-call will destroy it */
	STMFD SP!,{r1}
	
	/*
		Call routine to calculate location of context-store.
		We MUST save LR here. It is the return-address in User-mode
	*/
	STMFD SP!,{LR}
	BL _get_current_context_store
	LDMFD SP!,{LR}
	
	/* Restore r1 again */
	LDMFD SP!,{r1}


/*
	In the following we do the actual context-save and -restore.
	The memory in which the 17 registers are saved looks like this.

	Entrypoiny,SP,LR,r0,SPSR,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12
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
	STR r1, [r0, #16]
	
	/*
		Load the value of r0, which we previously saved on stack.
		SP is not affected during this
	*/
	LDR r1, [SP]
		
	/* Save r1 @ r0+12 */
	STR r1, [r0, #12]
		 
	/* Save current mode in r10 */
	MRS r10, CPSR
	
	/* Switch to system-mode */
	MOV r9, r10
	BIC r9,r9, #0xFF
	ORR r9, r9, #SYSTEM_MODE_NOIRQ /* System-mode and IRQ-disable - since pending interrupts would destroy operation */
	MSR CPSR, r9

	/* Move SP to r2 - so we can access it from interrupt-mode */
	MOV r2, SP
	
	/* Move LR to r3- so we can access it from interrupt-mode */
	MOV r3, LR
		
	/* Switch to previously saved mode */
	MOV r9, r10
	MSR CPSR, r9
	
	/* Save r2-r3 (SP, LR) at [r0 + 4] */
	ADD r0, r0, #4
	STMIA r0,{r2-r3}

_after_task_save:
	/* From this point on we have all registers to our self */

	LDR r5, =sched
	MOV LR, PC
	BX r5

	/* Here the process restore starts. WATCH THE REGISTERS */
	BL _get_current_context_store

	LDMIA r0!,{LR}
	LDMIA r0!,{r1-r2}
		
	/* Save current mode in r10 */
	MRS r10, CPSR
	
	/* Switch to system-mode */
	MOV r9, r10
	BIC r9,r9, #0xFF
	ORR r9, r9, #SYSTEM_MODE_NOIRQ /* System-mode and IRQ-disable - since pending interrupts would destry operation */
	MSR CPSR, r9

	/* Set SP and LR */
	MOV SP, r1
	MOV LR, r2
	
	/* Switch to previously saved mode */
	MOV r9, r10
	MSR CPSR, r9
	
	/* Load value of r0 */
	LDR r1, [r0]
	
	/* Save value on stack, which will be loaded later */
	STR r1, [SP]

	/* Load r1-r12 @ [r0+8] */
	ADD r0, r0, #8
	LDMIA r0,{r1-r12}

	/* Restore SPSR process-cpu-flags @ r0 - 4 */
	STMFD SP!,{r1} @ Store r1
	LDR r1, [r0, #-4]
	MSR SPSR, r1

	LDMFD SP!,{r1} @ Restore r1

@======================================================================================
_no_task_switch:

	STMFD SP!,{r1} @ Store r1

	@ Check if interrupts should be disabled
	LDR r1, =interrupts_disabled
	LDRB r1, [r1]
	CMP r1, #0
	BEQ interrupts_are_enabled

	MRS r1, SPSR
	ORR r1, r1, #0xC0  @ disable IRQ and FIQ interrupts
	MSR SPSR, r1
	B after_interrupt_endisable

interrupts_are_enabled:
	MRS r1, SPSR
	BIC r1, r1, #0xC0  @ enable IRQ and FIQ interrupts
	MSR SPSR, r1

after_interrupt_endisable:
	LDMFD SP!,{r1} @ Restore r1
	
return_from_irq:
	LDMFD SP!,{r0}
	MOVS PC, LR
