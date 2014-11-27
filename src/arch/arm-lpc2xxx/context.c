/*
	AOS - ARM Operating System
	Copyright (C) 2007  Søren Holm (sgh@sgh.dk)

	License: wxWindows Library Licence, Version 3.1
*/
#define AOS_KERNEL_MODULE

#include <arch/cpu.h>
#include <aos/kernel.h>

void syscall_ret(void);

void context_init(struct context* ctx, void* kstack) {
	struct kern_regs* k;
	struct cpu_regs* u;

	ctx->uregs = (struct cpu_regs*)(kstack - sizeof(struct cpu_regs));
	
	k = &ctx->kregs;
	u = ctx->uregs;

	k->lr = (uint32_t)syscall_ret;
	k->sp = (uint32_t)ctx->uregs;

	
	u->r0 = 0x00000000; // User arg
	u->r1 = 0x11111111;
	u->r2 = 0x22222222;
	u->r3 = 0x33333333;
	u->svc_sp = (uint32_t)kstack;

}

void context_destroy(struct context* ctx) {
	void* ptr = ((void*)ctx->uregs) + sizeof(struct cpu_regs);
	free(ptr);
}

void context_set(struct context* ctx, uint8_t type, uint32_t val) {
	struct cpu_regs* u;
	uint32_t cpsr = 0x0000001F; // System-mode
	//REGISTER_TYPE cpsr = 0x00000010; // User-mode

	u = ctx->uregs;

	switch (type) {
		case USER_STACK:
			u->sp = val;
			break;
		case USER_ENTRY:
			// If address is thumb we must set it in the PSR
			if ((val & 1) == 1)
				cpsr |= BIT5; // Set thumb bit
			u->pc = u->lr = val; // Entrypoint
			u->cpsr = cpsr;
			break;
		case USER_ARG:
			u->r0 = val; // User arg
			break;
		default:
			for (;;);
	}
}
