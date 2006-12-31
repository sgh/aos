
#ifndef GPIO_H
#define GPIO_H


#ifdef USE_GPIO

#include <stdio.h>
#include <types.h>

/**
 * \brief General Input-Output pin range.
 * 
 * Example: Port 2,  BIT4 to BIT6
 * <pre>
 * struct gpio testio = {
 *   .port = 2,
 *   .range = 3,
 *   .shifts = 4,
 * };
 * </pre>
 */
struct gpio {
	uint8_t port;			/**< \brief IO-port. */
	uint8_t range;		/**< \brief Length of bit-range. */
	uint8_t shifts;		/**< \brief Number of positions to shift the range. */
};

// #define __inline__  __attribute__((always_inline))

__inline__ static void gpio_set(struct gpio* io) {
	printf("port: %u  range: %u   shifts: %u\n",io->port,io->range,io->shifts);
	printf("GPIO%u_IOSET = 0x%X;\n",io->port, ((1<<io->range)-1) << io->shifts);
}

__inline__ static void gpio_clear(struct gpio* io) {
	printf("port: %u  range: %u   shifts: %u\n",io->port,io->range,io->shifts);
	printf("GPIO%u_IOCLR = 0x%X;\n",io->port, ((1<<io->range)-1) << io->shifts);
}

__inline__ static void gpio_get(struct gpio* io, uint32_t* data) {
	printf("port: %u  range: %u   shifts: %u\n",io->port,io->range,io->shifts);
	printf("*data = ((GPIO%u_PIN >> %u) & %u);\n",io->port, io->shifts, ((1<<io->range)-1));
}

__inline__ static void gpio_set_input(struct gpio* io) {
	printf("port: %u  range: %u   shifts: %u\n",io->port,io->range,io->shifts);
	printf("GPIO%u_IODIR &= ~(%u << %u);\n",io->port, ((1<<io->range)-1), io->shifts);
}

__inline__ static void gpio_set_output(struct gpio* io) {
	printf("port: %u  range: %u   shifts: %u\n",io->port,io->range,io->shifts);
	printf("GPIO%u_IODIR |= (%u << %u);\n",io->port, ((1<<io->range)-1), io->shifts);
}

__inline__ static void gpio_data(struct gpio* io, uint32_t data) {
	printf("port: %u  range: %u   shifts: %u\n",io->port,io->range,io->shifts);
	printf("GPIO%u_IOSET = ((data & %u) << %u);\n",io->port, ((1<<io->range)-1), io->shifts);
	printf("GPIO%u_IOCLR = (((~data) & %u) << %u);\n",io->port, ((1<<io->range)-1), io->shifts);
}

#endif

#endif
