/*
	AOS - ARM Operating System
	Copyright (C) 2007  Søren Holm (sgh@sgh.dk)

	License: wxWindows Library Licence, Version 3.1
*/
#ifndef AOS_CONFIG_H
#define AOS_CONFIG_H

#define HZ 1000

// The time quantum for each timeslice (ms)
#define QUANTUM 10

// The size of the kernel stack depends of which registers store in irq's and
// swi's. For ARM 19 register are stored - 19*4 = 76 bytes. So lets say that
// one interrupt occurs while in kernel-mode this is 76 bytes more => 152
// So a rule of thumb might be this :
//
// interrupt-frame size        : framsize
// Number of interrupt sources : numirq
// kernel stack-usage          : kernelstack
//
// KSIZE = (numirq+1)*framesize + kernstack
//
// - numirq+1 because a swi is also an interrupt-source
//
// Example: 
#define KSIZE (3+1) * (19*4) + 256

#endif
