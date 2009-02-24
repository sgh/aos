#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <aos/aosfifo.h>

// #define DEBUG_INPUT

#include <aos/input.h>

#ifndef DEBUG_INPUT
#define AOS_KERNEL_MODULE
#include <aos/kernel.h>
#include <aos/aos.h>
#include <aos/mutex.h>
#include <aos/irq.h>
#endif



static uint8_t queuedata[sizeof(AosEvent)*8 + 1];
static struct aos_fifo eventqueue;
#ifndef DEBUG_INPUT
static semaphore_t eventqueue_sem;
static mutex_t eventqueue_lock;
#endif

void dispatch_keypress(unsigned int scancode, unsigned int repeatcount) {
	AosEvent e;
	e.type = KeyPress;
	e.keyEvent.keycode = scancode;
	e.keyEvent.repeatcount = repeatcount;
	mutex_lock(&eventqueue_lock);
	aos_fifo_write(&eventqueue, &e, sizeof(AosEvent));
	mutex_unlock(&eventqueue_lock);
#ifndef DEBUG_INPUT
	sem_up(&eventqueue_sem);
#endif
}

void dispatch_keyrelease(unsigned int scancode) {
	AosEvent e;
	e.type = KeyRelease;
	e.keyEvent.keycode = scancode;
	e.keyEvent.repeatcount = 0;
	mutex_lock(&eventqueue_lock);
	aos_fifo_write(&eventqueue, &e, sizeof(AosEvent));
	mutex_unlock(&eventqueue_lock);
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
	mutex_init(&eventqueue_lock);
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
