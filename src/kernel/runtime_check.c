#define AOS_KERNEL_MODULE

#include <aos/kernel.h>
#include <string.h>
#include <aos/aos_hooks.h>
#include <stdio.h>
#include <aos/assert.h>
#include <aos/runtime_check.h>
#include <aos/task.h>

static const char const magic_string[3] = "AOS";

#define OFFSET 4 

void init_runtime_check(void) {
	memcpy((char*)&__stack_usr_bottom__ + OFFSET, magic_string, sizeof(magic_string));
	memcpy((char*)&__stack_svc_bottom__ + OFFSET, magic_string, sizeof(magic_string));
	memcpy((char*)&__stack_irq_bottom__ + OFFSET, magic_string, sizeof(magic_string));
	check_stack();
}

void check_stack(void) {
	unsigned int overflows = 0;

	// Check magic string at bottom of each stack
	if (memcmp((char*)&__stack_usr_bottom__ + OFFSET, magic_string, sizeof(magic_string))!=0)
		overflows |= STACK_USR;

	if (memcmp((char*)&__stack_svc_bottom__ + OFFSET, magic_string, sizeof(magic_string))!=0)
		overflows |= STACK_SVC;

	if (memcmp((char*)&__stack_irq_bottom__ + OFFSET, magic_string, sizeof(magic_string))!=0)
		overflows |= STACK_IRQ;

#ifdef SHARED_STACK
	// Next - check user stack-pointer for stack-button override
	/** @todo This will only catch user-mode stack overflow */
	if ((uint32_t)&__stack_usr_bottom__ > current->context->sp)
		overflows |= STACK_USR;
#endif

	sys_assert(!(overflows & STACK_SVC));

	sys_assert(!(overflows & STACK_USR));

	sys_assert(!(overflows & STACK_IRQ));
}
