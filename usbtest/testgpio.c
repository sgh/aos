#include <arm/lpc23xx.h>
#include <bits.h>
#include <aos.h>
#define AOS_KERNEL_MODULE
#include <syscalls.h>



// void sys_yield(void) {
// }

// _syscall1(void, msleep, uint16_t, ms)
// _syscall0(void, yield);

void main(void) {
	int i;

	aos_basic_init();
	PINSEL10 = 0;           /* Disable ETM interface, enable LEDs */
	FIO2DIR  = 0x000000FF;  /* P2.0..7 defined as Outputs         */
	FIO2MASK = 0x00000000;
	FIO2CLR  = 0xFF;

	for (;;) {
		FIO2CLR = 0xFFFFFFFF;
		FIO2SET = 0xFFFFFFFF;
	}
}

// AOS_MODULE_INIT(test_init);