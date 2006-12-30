#include <task.h>
#include <timer_interrupt.h>
#include <kernel.h>
#include <types.h>
#include <list.h>
#include <macros.h>


void timer_interrupt_routine() {
	struct task_t* t;
	struct list_head* e;
	uint32_t elapsed_time = uint32diff(last_interrupt_time, read_timer());
	uint32_t time_to_wake = MAX_TIME_SLICE_US;

	last_interrupt_time = read_timer();

	// If a one process is waiting, do context_switch
	if (!list_isempty(&readyQ))
		do_context_switch = 1; // Signal context-switch
	
	// If someone is sleeping
	if (!list_isempty(&usleepQ)) {
		e = list_get_front(&usleepQ);
		t = get_struct_task(e);

		if (t->sleep_time) { // If process had time left to sleep
			if (t->sleep_time > elapsed_time)
				t->sleep_time -= elapsed_time;
			else
				t->sleep_time = 0;
		}

		if (t->sleep_time == 0) { // If process now has no time left to sleep
			struct task_t* next = get_struct_task(list_get_front(&readyQ));
			list_erase(&t->q);
			list_push_front(&readyQ,&t->q);
			if (t->priority > next->priority)
				do_context_switch = 0; // Un-signal context-switch
		}

		if (!list_isempty(&usleepQ)) {
			e = list_get_front(&usleepQ); // Set e to the next to wake up
			t = get_struct_task(e); // Get task-struct.

			time_to_wake = t->sleep_time;
			if (time_to_wake < MIN_TIME_SLICE_US) // Make sure that timeslice stays above MIN_TIME_SLICE_US
				time_to_wake = MIN_TIME_SLICE_US;

			if (time_to_wake > MAX_TIME_SLICE_US) // Make sure that timeslice stays below MAX_TIME_SLICE_US
				time_to_wake = MAX_TIME_SLICE_US;
		}

	}

	set_timer_match( read_timer() + time_to_wake );
	clear_timer_interrupt();
		}

