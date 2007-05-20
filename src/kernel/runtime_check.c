#define AOS_KERNEL_MODULE

#include <kernel.h>
#include <string.h>
#include <aos_hooks.h>
#include <stdio.h>

static const char const magic_string[3] = "AOS";

void init_runtime_check(void) {
	memcpy(&__stack_usr_bottom__, magic_string, sizeof(magic_string));
	memcpy(&__stack_svc_bottom__, magic_string, sizeof(magic_string));
	memcpy(&__stack_irq_bottom__, magic_string, sizeof(magic_string));
}

void check_stack(void) {
	unsigned int overflows = 0;
	if (memcmp(&__stack_usr_bottom__, magic_string, sizeof(magic_string))!=0)
		overflows |= STACK_USR;

	if (memcmp(&__stack_svc_bottom__, magic_string, sizeof(magic_string))!=0)
		overflows |= STACK_SVC;

	if (memcmp(&__stack_irq_bottom__, magic_string, sizeof(magic_string))!=0)
		overflows |= STACK_IRQ;

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

		AOS_FATAL(buf);
	}
}
