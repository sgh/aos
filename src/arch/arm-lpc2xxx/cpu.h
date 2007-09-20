/*
		AOS - ARM Operating System
		Copyright (C) 2007  Søren Holm (sgh@sgh.dk)

		This library is free software; you can redistribute it and/or
		modify it under the terms of the GNU Lesser General Public
		License as published by the Free Software Foundation; either
		version 2.1 of the License, or (at your option) any later version.

		This library is distributed in the hope that it will be useful,
		but WITHOUT ANY WARRANTY; without even the implied warranty of
		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
		Lesser General Public License for more details.

		You should have received a copy of the GNU Lesser General Public
		License along with this library; if not, write to the Free Software
		Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
		*/
#ifndef CPU_H
#define CPU_H


struct cpu_regs {
	uint32_t r0;	/*  +0 (00) */
	uint32_t r1;	/*  +4 (04) */
	uint32_t r2;	/*  +8 (08) */
	uint32_t r3;	/* +12 (0C) */
	uint32_t r4;	/* +16 (10) */
	uint32_t r5;	/* +20 (14) */
	uint32_t r6;	/* +24 (18) */
	uint32_t r7;	/* +28 (1C) */
	uint32_t r8;	/* +32 (20) */
	uint32_t r9;	/* +36 (24) */
	uint32_t r10;	/* +40 (28) */
	uint32_t r11; 	/* +44 (2C) */
	uint32_t r12;	/* +48 (30) */
	uint32_t sp;	/* +52 (34) */
	uint32_t lr;	/* +56 (38) */
	uint32_t svc_sp;	/* +60 (3C) */
	uint32_t svc_lr;	/* +64 (40) */
	uint32_t pc;	/* +68 (44) */
	uint32_t cpsr;	/* +72 (48) */
};

struct kern_regs {
	uint32_t r4;
	uint32_t r5;
	uint32_t r6;
	uint32_t r7;
	uint32_t r8;
	uint32_t r9;
	uint32_t r10;
	uint32_t r11;
	uint32_t sp;
	uint32_t lr;
};

struct context {
	struct kern_regs kregs;
	struct cpu_regs* uregs;
};

#endif
