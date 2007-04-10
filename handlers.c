#include <arm/lpc23xx.h>

/*
 * Prototypes used to set attribute
 */
void Undef_Handler(void) __attribute__ ((interrupt));
void SWI_Handler(void)   __attribute__ ((interrupt));
void PAbt_Handler(void)  __attribute__ ((interrupt));
void DAbt_Handler(void)  __attribute__ ((interrupt));
void IRQ_Handler(void)   __attribute__ ((interrupt));
void FIQ_Handler(void)   __attribute__ ((interrupt));

/*
 * Implementation
 */
void Undef_Handler(void)
{
	while (1)
		;
}

void SWI_Handler(void)
{
	while (1)
		;
}

void PAbt_Handler(void)
{
	while (1)
		;
}

void DAbt_Handler(void)
{
	while (1)
		;
}
/*
void IRQ_Handler(void)
{

}
*/
void FIQ_Handler(void)
{

}


