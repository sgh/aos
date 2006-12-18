.global atomic_xchg


@ uint16 atomic_xchg(uint16* address, uint16 new_val)
atomic_xchg:
	STMFD SP!,{r1-r3}
	MOV r3, r0
	SWP r0, r1, [r3]
	LDMFD SP!,{r1-r3}
	BX LR

