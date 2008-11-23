#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define AOS_KERNEL_MODULE
#include <aos/kernel.h>
#include <aos/aos.h>
#include <aos/mutex.h>
#include <aos/input.h>
#include <aos/irq.h>

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
	unsigned int getidx = fifo->getidx;

// 	printf("Reading %d bytes ...", len);

	while (len-- && getidx != fifo->putidx) {
// 	printf("[%d]=0x%02X ", getidx, ucsrc[getidx]);
		*ucdst = ucsrc[getidx];
		ucdst++;
		getidx++;
		if (getidx >= fifo->size)
			getidx = 0;
	}
// 	printf("\n");

	return 0;
}

int aos_fifo_write(struct aos_fifo* fifo, void* src, int len) {
	uint8_t *ucsrc = (uint8_t*)src;
	uint8_t *ucdst = (uint8_t*)fifo->data;
	unsigned int putidx = fifo->putidx;
	int count = len;

// 	printf("Adding %d bytes ...", len);
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

// 		printf("[%d]=0x%02X ", putidx, *ucsrc);
		
		ucdst[putidx] = *ucsrc;
		ucsrc++;
		
	}
// 	printf(" :: %d bytes\n", (len - (count + 1)));
	fifo->putidx = putidx;

	return (len - count);
}

#define KeyPress   0
#define KeyRelease 1

struct KeyEvent {
	int type;
	unsigned int keycode;
};

union _AosEvent {
	int type;
	struct KeyEvent keyEvent;
};

typedef union _AosEvent AosEvent;

static uint8_t queuedata[sizeof(AosEvent)*16];
static struct aos_fifo eventqueue;
static semaphore_t eventqueue_sem;

void dispatch_keypress(int scancode) {
	AosEvent e;
	e.type = KeyPress;
	e.keyEvent.keycode = scancode;
	aos_fifo_write(&eventqueue, &e, sizeof(AosEvent));
	sem_up(&eventqueue_sem);
}

void dispatch_keyrelease(int scancode) {
	AosEvent e;
	e.type = KeyRelease;
	e.keyEvent.keycode = scancode;
	aos_fifo_write(&eventqueue, &e, sizeof(AosEvent));
	sem_up(&eventqueue_sem);
}

void eventqueue_init(void);
void eventqueue_init(void)
{
	sem_init(&eventqueue_sem, 0);
	aos_fifo_init(&eventqueue, queuedata, sizeof(queuedata));
}