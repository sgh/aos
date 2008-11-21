#include <stdint.h>
#include <stdio.h>

#define KeyPressed  0
#define KeyReleased 0

struct aos_fifo {
	unsigned int putidx;
	unsigned int getidx;
	unsigned int size;
	void* data;
};

int aos_fifo_init(struct aos_fifo* fifo, void* data, int len) {
	fifo->putidx = 0;
	fifo->getidx = 0;
	fifo->size = len;
	fifo->data = data;
}

int aos_fifo_read(struct aos_fifo* fifo, void* dst, int len) {
	uint8_t *ucdst = (uint8_t*)dst;
	uint8_t *ucsrc = (uint8_t*)fifo->data;
	int getidx = fifo->getidx;

	printf("Reading %d bytes ...", len);

	while (len-- && getidx != fifo->putidx) {
	printf("[%d]=0x%02X ", getidx, ucsrc[getidx]);
		*ucdst = ucsrc[getidx];
		ucdst++;
		getidx++;
		if (getidx >= fifo->size)
			getidx = 0;
	}
	printf("\n");

	return;
}

int aos_fifo_write(struct aos_fifo* fifo, void* src, int len) {
	uint8_t *ucsrc = (uint8_t*)src;
	uint8_t *ucdst = (uint8_t*)fifo->data;
	int putidx = fifo->putidx;
	int count = len;

	printf("Adding %d bytes ...", len);
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

		printf("[%d]=0x%02X ", putidx, *ucsrc);
		
		ucdst[putidx] = *ucsrc;
		ucsrc++;
		
	}
	printf(" :: %d bytes\n", (len - (count + 1)));
	fifo->putidx = putidx;

	return (len - count);
}

char testdata[8];
struct aos_fifo fifo;

void testfifo() {
	char buffer[8];
	aos_fifo_init(&fifo, testdata, sizeof(testdata));
	aos_fifo_write(&fifo, "ABCD", 4);
	aos_fifo_write(&fifo, "EFGH", 4);
	aos_fifo_read(&fifo, buffer, sizeof(buffer));
// 	aos_fifo_write(&fifo, "EFGH", 4);
// 	aos_fifo_write(&fifo, "IJKL", 4);
// 	aos_fifo_write(&fifo, "MNOP", 4);
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
