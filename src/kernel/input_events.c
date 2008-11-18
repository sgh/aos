#include <stdint.h>
#include <stdio.h>

#define KeyPressed  0
#define KeyReleased 0

struct aos_fifo {
	unsigned int put;
	unsigned int get;
	unsigned int size;
	uint8_t* data;
};

int aos_fifo_init(struct aos_fifo* fifo, void* data, int len) {
	fifo->put = 0;
	fifo->get = 0;
	fifo->size = len;
	fifo->data = data;
}

int aos_fifo_read(void* dst, struct aos_fifo* fifo, int len) {
	uint8_t *ucdst = (uint8_t*)dst;
	uint8_t *ucsrc = (uint8_t*)fifo->data;
	int getidx = fifo->get;

	while (len-- && getidx != fifo->putidx) {
		*ucdst = ucsrc[getidx]
		ucdst++;
		getidx++;
		if (getidx >= fifo->size)
			getidx = 0;
	}

	return;
}

int aos_fifo_write(struct aos_fifo* fifo, void* src, int len) {
	uint8_t *ucsrc = (uint8_t*)src;
	uint8_t *ucdst = (uint8_t*)fifo->data;
	int putidx = fifo->put;

	while (len-- && putidx != fifo->getidx) {
		ucdst[putidx] = *ucsrc
		ucsrc++;
		putidx++;
		if (putidx >= fifo->size)
			getidx = 0;
	}

	return;
}

#warning implement eventqueue here
struct KeyEvent {
	int type;
	unsigned int keycode;
};

union {
	int type;
	struct KeyEvent keyEvent;
} AosEvent;

void dispatch_keypress(int scancode) {
	printf("Keypress %d\n", scancode);
}

void dispatch_keyrelease(int scancode) {
	printf("Keyrelease %d\n", scancode);
}
