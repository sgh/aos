
.extern return_from_interrupt

/* extern symbols */
.extern sched
.extern current
.extern do_task_switch

/* Extern C interrupt-handlers */
.extern timer_interrupt_routine
.extern uart0_interrupt_routine


.equ SYSTEM_MODE_NOIRQ, 0xDF @ System-mode with IRQ an FIRQ disabled
.equ SYSTEM_MODE_IRQ, 0xDC @ System-mode with IRQ an FIRQ enabled


/* IRQ prologue- and epilogue macros */
.macro IRQ_prologue
		/* Save r0-r12,LR on User-mode stack */
	STMFD SP!,{r0-r12,LR}	
.endm

.macro IRQ_epilogue
	/* Restore r0-r12,LR frim User-mode-stack */
	LDMFD SP!,{r0-r12,LR}
	
	/* Substract 4 from return addres - we do this to correctly return in a general matter */
	SUB LR, LR, #4
	B return_from_interrupt
.endm
