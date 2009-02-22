#include <aos/aosfifo.h>
#include <stdint.h>

void aos_fifo_init(struct aos_fifo* fifo, void* data, int len) {
	fifo->putidx = 0;
	fifo->getidx = 0;
	fifo->size = len;
	fifo->data = data;
#ifdef DEBUG_INPUT
	printf("len:%d\n", len);
#endif
}

int aos_fifo_read(struct aos_fifo* fifo, void* dst, int len) {
	uint8_t *ucdst = (uint8_t*)dst;
	uint8_t *ucsrc = (uint8_t*)fifo->data;
	unsigned int getidx = fifo->getidx;

#ifdef DEBUG_INPUT
	printf("Reading %d bytes ...", len);
#endif
	while (len-- && getidx != fifo->putidx) {
		getidx++;
		if (getidx >= fifo->size)
			getidx = 0;
#ifdef DEBUG_INPUT
		printf("[%d]=0x%02X ", getidx, ucsrc[getidx]);
#endif
		*ucdst = ucsrc[getidx];
		ucdst++;
	}
	fifo->getidx = getidx;
#ifdef DEBUG_INPUT
	//printf("\n");
#endif

	return 0;
}

int aos_fifo_write(struct aos_fifo* fifo, void* src, int len) {
	uint8_t *ucsrc = (uint8_t*)src;
	uint8_t *ucdst = (uint8_t*)fifo->data;
	unsigned int putidx = fifo->putidx;
	int count = len;

#ifdef DEBUG_INPUT
	printf("Adding %d bytes ...", len);
#endif
	while (count--) {
		putidx++;
		if (putidx == fifo->size)
			putidx = 0;

		if (putidx == fifo->getidx) {
			if (putidx == 0)
				putidx = fifo->size - 1;
			else
				putidx--;
			break;
		}

#ifdef DEBUG_INPUT
		printf("[%d]=0x%02X ", putidx, *ucsrc);
#endif
		
		ucdst[putidx] = *ucsrc;
		ucsrc++;
	}
#ifdef DEBUG_INPUT
	printf(" :: %d bytes\n", (len - (count + 1)));
#endif
	fifo->putidx = putidx;

	return (len - count);
}

