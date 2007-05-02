/*****************************************************************************
 *
 * Project          : GNUEasyWeb
 * Subproject       : 
 * Name             : init_cpu.c
 * Function         : Initialisation of CPU modules
 * Designer         : K. Sterckx
 * Creation date    : 19/01/2007
 * Compiler         : GNU ARM
 * Processor        : LPC2368
 * Last update      :
 * Last updated by  :
 * History          :
 *
 *****************************************************************************
 *
 *  Called from startarm.s
 *
 *  Interrupts
 *
 *   FIQ       FIQ_Handler(handlers.c)
 *   IRQ
 *
 ***************************************************************************/

#include <arm/lpc23xx.h>

/*****************************************************************************
 * Defines the clock values in Hz
 *
 *  Change when
 *   - crystal changes
 *   - Init_CPU code changes
 */
#define RC_FREQUENCY       4000000
#define CPU_CLK_FREQUENCY 72000000

/*****************************************************************************
 * Defines the wait values used in Init_Wait
 *  Depends on CPU clk
 *  Must result in a wait time of around 500mS
 */
#define CONFIG_INITWAIT_J  20
#define CONFIG_INITWAIT_I  (RC_FREQUENCY / 1000)

/*****************************************************************************
 * Used to halt the CPU a few ms so that JTAG can take control
 *  over processor before any other code is called.
 *
 * Called with interrupts disabled and before stack and variables are setup
 */
static void wait(void)
{
}

void Init_Wait(void)
{
	unsigned int i;
	unsigned int j;

	for (j = 0; j < CONFIG_INITWAIT_J; j++)
	{
		for (i = 0; i < CONFIG_INITWAIT_I; i++)
		{
			wait();
		}
	}
}

/*****************************************************************************
 * Used to initialize the Clocks
 *
 * Called with interrupts disabled and before stack and variables are setup
 *
 *  Crystal is 12.000MHz, this is the main clock, Fin
 *  PLL will be clocked from the main clock, we want a Fcco clock of 480MHz
 *   so it can be used to generate 60MHz for CPU and 48MHz for USB.
 *   Fcco = (2*M*Fin)/N, M=12, N=1 -> Fcco = 288MHz
 *   (Value M-1 and N-1 shall be written to PLL )
 *
 *  Fcpu = 72MHz = 288MHz/4
 *  Fusb = 48MHz = 288MHz/6
 *
 *  Fperipheral = Fcpu/4 for all, can be changed in Init_Modules
 *  Resulting in a PCLK of 18MHz for all unless changed later.
 *
 *  Remark
 *   Unable to get oscillator running for CPU=60Mhz (M=20,N=1), because
 *   of bug in LPC, see errata sheet (Fosc limited to 290MHz)
 */
void Init_Clocks(void)
{
	/* Start main oscillator */
// 	SCS |= (1<<5);

// 	while ((SCS & (1<<6)) == 0)
// 		;

	/* Select PLL as source */
// 	CLKSRCSEL = 0x01;

	/* Start PLL */
	PLLCFG    = 0x00000025;  // Value M-1 and N-1
	PLLFEED   = 0xAA;
	PLLFEED   = 0x55;

	PLLCON    = 0x01;
	PLLFEED   = 0xAA;
	PLLFEED   = 0x55;

	/* Wait until locked */
	while ((PLLSTAT & (1<<10)) == 0)
		;

	/* Wait until M and N are correct */
// 	while ((PLLSTAT & 0xFF7FFF) != 0x0000000B)
// 		;

	/* Setup CPU clock divider */
// 	CCLKCFG = 3;

	/* Setup USB clock divider */
// 	USBCLKCFG = 5;

	/* Setup Peripheral clock */
// 	PCLKSEL0 = 0;
// 	PCLKSEL1 = 0;

	/* Switch to PLL clock */
	PLLCON   = 0x03;
	PLLFEED  = 0xAA;
	PLLFEED  = 0x55;

	/* Make sure we are connected to PLL */
// 	while ((PLLSTAT & (1<<25))==0)
// 		;
}

/*****************************************************************************
 * Used to initialize the MAM
 *
 * Called with interrupts disabled and before stack and variables are setup
 */
void Init_MAM(void)
{
	MAMCR  = 0x00;  /* Off                                        */
	MAMTIM = 0x04;  /* 4 Fetch cycles, gives better result than 3 */
	MAMCR  = 0x02;  /* Fully enabled                              */

	MEMMAP = 0x01;  /* Uses interrupts vectors in flash */
}

/*****************************************************************************
 * Used to initializes the CPU
 *
 *  Some special setup before interrupts are enabled
 *
 * Called with interrupts disabled, stack and variables are setup
 */
void Init_CPU(void)
{
}

/*****************************************************************************
 * Used to initialize the Pins
 *
 *  Pins can also be setup in Startarm.s
 *
 * Called with interrupts disabled and before stack and variables are setup
 */
void Init_Pins(void)
{
	/* Setup P2.0 - P2.7 as outputs, disconnect them from ETM */
//	PINSEL10 = 0;
//	FIO2DIR  = 0x000000FF;
//	FIO2MASK = 0x00000000;
//	FIO2CLR  = 0xFF;

	/* Enable FAST mode for PORT0 and PORT1 */
//	SCS |= (1<<0);
}

/*****************************************************************************
 * Used to initializes the I/O Modules
 *
 * Called with interrupts disabled, stack and variables are setup
 */
void Init_Modules(void)
{

}

/*****************************************************************************
 * Called when main is exited
 */
void Exit_Main(void)
{
}

