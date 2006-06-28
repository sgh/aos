
#ifndef GPIO_H
#define GPIO_H

#include <stdio.h>
#include <types.h>

struct gpio {
	uint8 port;			// IO-port
	uint8 range;		// Length of bit-range
	uint8 shifts;		// Number of positions to shift the range
};

// #define __inline__  __attribute__((always_inline))

static void __inline__ gpio_set(struct gpio* io) {
	printf("port: %u  range: %u   shifts: %u\n",io->port,io->range,io->shifts);
	printf("GPIO%u_IOSET = 0x%X;\n",io->port, ((1<<io->range)-1) << io->shifts);
}

static void __inline__ gpio_clear(struct gpio* io) {
	printf("port: %u  range: %u   shifts: %u\n",io->port,io->range,io->shifts);
	printf("GPIO%u_IOCLR = 0x%X;\n",io->port, ((1<<io->range)-1) << io->shifts);
}

static void __inline__ gpio_get(struct gpio* io, uint32* data) {
	printf("port: %u  range: %u   shifts: %u\n",io->port,io->range,io->shifts);
	printf("*data = ((GPIO%u_PIN >> %u) & %u);\n",io->port, io->shifts, ((1<<io->range)-1));
}

static void __inline__ gpio_highimpedance(struct gpio* io) {
	printf("port: %u  range: %u   shifts: %u\n",io->port,io->range,io->shifts);
	printf("GPIO%u_IODIR &= ~(%u << %u);\n",io->port, ((1<<io->range)-1), io->shifts);
}

static void __inline__ gpio_lowimpedance(struct gpio* io) {
	printf("port: %u  range: %u   shifts: %u\n",io->port,io->range,io->shifts);
	printf("GPIO%u_IODIR |= (%u << %u);\n",io->port, ((1<<io->range)-1), io->shifts);
}

static void __inline__ gpio_data(struct gpio* io, uint32 data) {
	printf("port: %u  range: %u   shifts: %u\n",io->port,io->range,io->shifts);
	printf("GPIO%u_IOSET = ((data & %u) << %u);\n",io->port, ((1<<io->range)-1), io->shifts);
	printf("GPIO%u_IOCLR = (((~data) & %u) << %u);\n",io->port, ((1<<io->range)-1), io->shifts);
}

#endif
