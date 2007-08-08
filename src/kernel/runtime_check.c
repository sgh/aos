#define AOS_KERNEL_MODULE

#include <kernel.h>
#include <string.h>
#include <aos_hooks.h>
#include <stdio.h>
#include <assert.h>

static const char const magic_string[3] = "AOS";

void init_runtime_check(void) {
	memcpy(&__stack_usr_bottom__, magic_string, sizeof(magic_string));
	memcpy(&__stack_svc_bottom__, magic_string, sizeof(magic_string));
	memcpy(&__stack_irq_bottom__, magic_string, sizeof(magic_string));
}

void check_stack(void) {
	unsigned int overflows = 0;

	// Check magic string at bottom of each stack
	if (memcmp(&__stack_usr_bottom__, magic_string, sizeof(magic_string))!=0)
		overflows |= STACK_USR;

	if (memcmp(&__stack_svc_bottom__, magic_string, sizeof(magic_string))!=0)
		overflows |= STACK_SVC;

	if (memcmp(&__stack_irq_bottom__, magic_string, sizeof(magic_string))!=0)
		overflows |= STACK_IRQ;

	// Next - check user stack-pointer for stack-button override
	/** @todo This will only catch user-mode stack overflow */
	if ((uint32_t)&__stack_usr_bottom__ > current->context->sp)
		overflows |= STACK_USR;


	if (overflows) {
		int idx = 0;
		char buf[32];
		idx += sprintf(buf, "Stack overflow ");
		
		if (overflows & STACK_USR)
			idx += sprintf(buf+idx, " USR");

		if (overflows & STACK_SVC)
			idx += sprintf(buf+idx, " SVC");

		if (overflows & STACK_IRQ)
			idx += sprintf(buf+idx, " IRQ");

// 		AOS_FATAL(buf);
		sys_assert(FALSE);
	}
}
