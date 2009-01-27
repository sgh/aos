#include <stdint.h>
#include <stdio.h>
#include <string.h>

// #define DEBUG_INPUT

#include <aos/input.h>

#ifndef DEBUG_INPUT
#define AOS_KERNEL_MODULE
#include <aos/kernel.h>
#include <aos/aos.h>
#include <aos/mutex.h>
#include <aos/irq.h>
#endif


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


static uint8_t queuedata[sizeof(AosEvent)*8 + 1];
static struct aos_fifo eventqueue;
#ifndef DEBUG_INPUT
static semaphore_t eventqueue_sem;
#endif

void dispatch_keypress(unsigned int scancode, unsigned int repeatcount) {
	AosEvent e;
	e.type = KeyPress;
	e.keyEvent.keycode = scancode;
	e.keyEvent.repeatcount = repeatcount;
	aos_fifo_write(&eventqueue, &e, sizeof(AosEvent));
#ifndef DEBUG_INPUT
	sem_up(&eventqueue_sem);
#endif
}

void dispatch_keyrelease(unsigned int scancode) {
	AosEvent e;
	e.type = KeyRelease;
	e.keyEvent.keycode = scancode;
	e.keyEvent.repeatcount = 0;
	aos_fifo_write(&eventqueue, &e, sizeof(AosEvent));
#ifndef DEBUG_INPUT
	sem_up(&eventqueue_sem);
#endif
}

int aos_get_event(AosEvent* e, int timeout) {
	int result = ESUCCESS;	
#ifndef DEBUG_INPUT
	if (timeout >= 0)
		result = sem_timeout_down(&eventqueue_sem, timeout);
	else
		sem_down(&eventqueue_sem);
#endif
	if (result == ESUCCESS)
		aos_fifo_read(&eventqueue, e, sizeof(AosEvent));

#ifdef DEBUG_INPUT
	printf("type:%d keycode:%d\n", e->type, e->keyEvent.keycode);
#endif
	return result;
}

void eventqueue_init(void);
void eventqueue_init(void)
{
#ifndef DEBUG_INPUT
	sem_init(&eventqueue_sem, 0);
#endif
	aos_fifo_init(&eventqueue, queuedata, sizeof(queuedata));
}

#ifdef DEBUG_INPUT
int main() {
	AosEvent e;
	eventqueue_init();

	while (1) {
		dispatch_keypress(5, 0);
		aos_get_event(&e);
		if (e.type == 0)
			break;
	}
}
#endif
