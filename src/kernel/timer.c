#include <aos/timer.h>
#include <aos/types.h>
#include <aos/list.h>
#include <aos/macros.h>
#include <aos/irq.h>

// #define DEBUG


#define PROCESS_ALL_EVENTS

#ifdef DEBUG
#include <stdio.h>
#endif

/**
 * Timer types
 * 1. Periodic timer
 * 2. One-shot-timer
 */

#define EVENT_INIT(var,_name) { .waitq = LIST_HEAD_INIT(var.waitq), .name = _name }

// #define time_after_eq(a,b) ((long)(a) - (long)(b) >= 0)
// #define time_before(a,b) ((long)(a) - (long)(b) < 0)

#define time_after_eq(a,b) ((long)(a) >= (long)(b))
#define time_before(a,b) ((long)(a) < (long)(b))

struct event {
	char* name;
	struct list_head waitq;	
};

static volatile uint32_t next_expire;
static volatile uint8_t  timer_active;

volatile uint32_t system_ticks;


static struct list_head timer_list = LIST_HEAD_INIT(timer_list);
static struct event timer_event = EVENT_INIT(timer_event,"timer");


#define get_struct_timer(t) container_of(t, struct timer, node)

void timer_stop(struct timer* tmr) {
	
	if (tmr->type == TMR_STOP)
		return;

	list_erase(&tmr->node);
	tmr->type = TMR_STOP;
	
	if (!list_isempty(&timer_list)) {
		struct timer* t = get_struct_timer(list_get_front(&timer_list));

		next_expire = t->expire;
	} else
		timer_active = 0;
	
// 	if (timer_active)
// 		printf("_expire @ tick %d\n", next_expire);
}


static void timer_setup(struct timer* tmr, uint32_t ticks) {
	struct list_head* it;
	uint32_t tmp_system_ticks = system_ticks;
	uint32_t expire;
	struct timer* t;

	if (tmr->type != TMR_STOP)
		timer_stop(tmr);

	expire = tmp_system_ticks + ticks;
#ifdef DEBUG
	printf("sys:%d ticks:%d %d ",tmp_system_ticks, ticks, expire);
#endif

	list_for_each(it, &timer_list) {
		t = get_struct_timer(it);
		if (time_before(expire, t->expire))
			break;
	}
	
	list_push_back(it, &tmr->node);
	
	tmr->expire = expire;

	t = get_struct_timer(list_get_front(&timer_list));
	next_expire = t->expire;

#ifdef DEBUG
	printf("expire @ tick %d %s\n", next_expire, overflow ? "OVR" : "");
#endif
	
	timer_active = 1;
}

static void handle_timer_event(void) {
#ifndef DEBUG
	irq_lock();
#endif
	struct timer* tmr = get_struct_timer(list_get_front(&timer_list));
#ifndef DEBUG
	irq_unlock();
#else
	printf("EXPIRE\n");
#endif

	switch (tmr->type) {
		case TMR_TIMEOUT :
#ifdef DEBUG
 			printf("Timeout\n");
#endif
			tmr->func(tmr->arg);
			timer_stop(tmr);
			break;
// 		case TMR_PERIODIC :
// 			printf("Periodic");
// 			break;
// 		default:
// 			printf("Unknown timer\n");
	}

}


void timer_clock(void);
	
void timer_clock(void) {
#ifdef PROCESS_ALL_EVENTS
	while (likely(timer_active) && time_after_eq(system_ticks, next_expire))
		handle_timer_event();
#else
	if (likely(timer_active)) {
		if ( time_after_eq(system_ticks, next_expire))
			handle_timer_event();
	}
#endif
}

void timer_timeout(struct timer* tmr, void (*func)(void*), void* arg, uint32_t ticks) {
	tmr->func = func;
	tmr->arg = arg;
	timer_setup(tmr, ticks);
	tmr->type = TMR_TIMEOUT;
}


#ifdef DEBUG

void test_hook(void* argh) {
	printf("Handler\n");
}

void invalid_test_hook(void* argh) {
	printf("Msut not be called\n");
}

int main(void) {
	struct timer tmr1;
	struct timer tmr2;

	tmr1.type = TMR_STOP;
	tmr2.type = TMR_STOP;

	while (1) {
		printf(".");
		fflush(0);
		usleep(500000);
		system_ticks++;
		timer_clock();
// 		if (system_ticks == 2)
// 			timer_timeout(&tmr2, &test_hook, NULL, 2);
		if (system_ticks == 4)
			timer_timeout(&tmr1, &invalid_test_hook, NULL, UINT32_MAX-1);
		if (system_ticks == 10)
			timer_timeout(&tmr2, &test_hook, NULL, 2);
	}
}
#endif
