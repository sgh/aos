#include <timer.h>
#include <types.h>
#include <list.h>
#include <macros.h>
#include <irq.h>

/**
 * Timer types
 * 1. Periodic timer
 * 2. One-shot-timer
 */

#define EVENT_INIT(var,_name) { .waitq = LIST_HEAD_INIT(var.waitq), .name = _name }

#define time_after_eq(a,b) ((long)(a) - (long)(b) >= 0)
#define time_before(a,b) ((long)(a) - (long)(b) < 0)

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
	
	if (!list_isempty(&timer_list))
		next_expire = get_struct_timer(list_get_front(&timer_list))->expire;
	else
		timer_active = 0;
	
// 	if (timer_active)
// 		printf("_expire @ tick %d\n", next_expire);
}


static void timer_setup(struct timer* tmr, uint32_t ticks) {
	struct list_head* it;
	uint32_t expire = ticks + system_ticks;
	struct timer* t;
	
	if (tmr->type != TMR_STOP)
		timer_stop(tmr);
	
	list_for_each(it, &timer_list) {
		t = get_struct_timer(it);
		if (time_before(expire, t->expire))
			break;
	}
	
	list_push_back(it, &tmr->node);
	
	tmr->expire = expire;
	
	next_expire = get_struct_timer(list_get_front(&timer_list))->expire;
	
// 	printf("expire @ tick %d\n", next_expire);
	
	timer_active = 1;
}

static void handle_timer_event(void) {
	irq_lock();
	struct timer* tmr = get_struct_timer(list_get_front(&timer_list));
	irq_unlock();
// 	printf("EXPIRE\n");

	switch (tmr->type) {
		case TMR_TIMEOUT :
// 			printf("Timeout\n");
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
	if (timer_active) {
		if (time_after_eq(system_ticks, next_expire))
			handle_timer_event();
	}
}

void timer_timeout(struct timer* tmr, void (*func)(void*), void* arg, uint32_t expire) {
	tmr->func = func;
	tmr->arg = arg;
	timer_setup(tmr, expire);
	tmr->type = TMR_TIMEOUT;
}


// int main(void) {
// 	struct timer tmr1;
// 	struct timer tmr2;
// 	
// 	tmr1.type = TMR_STOP;
// 	tmr2.type = TMR_STOP;
// 	
// 	while (1) {
// 		usleep(100000);
// 		timer_clock();
// 		if (system_ticks == 2)
// 			timer_timeout(&tmr1, sleep_timeout, NULL, 10);
// 		if (system_ticks == 4)
// 			timer_timeout(&tmr2, sleep_timeout, NULL, 2);
// 	}
// }
