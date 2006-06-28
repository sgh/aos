#include <queue.h>
#include <macros.h>
#include <task.h>
#include <stdio.h>
#include <types.h>

struct list_head readyQ;
struct list_head msleepQ;
struct list_head usleepQ;

void list_push_front(struct list_head *q, struct list_head* qe) {
// 	if (q->next)
		q->next->prev = qe;	// Point front back to new front
	qe->next = q->next;		// Point new front to old front
	qe->prev = q;			// Point new front back to list_head
	q->next = qe;					// Set new front to qe
// 	if (!q->prev)
// 		q->prev = qe;
}

void list_push_back(struct list_head* q, struct list_head* qe) {
// 	if (q->prev)
		q->prev->next = qe;		// Point back to new back
	qe->prev = q->prev;			// Point new back back to old back
	qe->next = q;				// Point new back to list_head
	q->prev = qe;						// Set new back to qe
// 	if (!q->next)
// 		q->next = qe;
}

uint8 list_isempty(struct list_head* q) {
	return !(q->next == q);
}

struct list_head* list_get_back(struct list_head* q) {
	struct list_head* prev = q->prev;
	
// 	if (prev == q)
// 		prev = NULL;
		
	/*if (!back) return NULL;
	
	if (back->prev)
		back->prev->next = NULL;
	else
		q->front = NULL;
	q->back = back->prev;
	back->next = NULL;
	back->next = NULL;*/
	
	return prev;
}

struct list_head* list_get_front(struct list_head* q) {
	struct list_head* next = q->next;
	
// 	if (next == q)
// 		next = NULL;
	
	
	/*if (!front) return NULL;
	
	if (front->next)
		front->next->prev = NULL;
	else
		q->back = NULL;
	q->front = front->next;
	front->next = NULL;
	front->prev = NULL;*/
	
	return next;
}

void list_erase(struct list_head* q, struct list_head* qe) {

	/* First we point previous and next entries to each other */
// 	if (qe->next)
		qe->next->prev = qe->prev;
	
// 	if (qe->prev)
		qe->prev->next = qe->next;

	/* Then we check to see if the entry happenened to be  front or back */
// 	if (q->next == qe)
// 		q->next = qe->next;
	
// 	if (q->prev == qe)
// 		q->prev = qe->prev;
		
	/* Then we reset the actual entry's next and prev */
	qe->next = 0;
	qe->prev = 0;

}

/*LIST_HEAD(readyQ);

struct task_t one = {
	.name = "one",
};

struct task_t two = {
	.name = "two",
};

struct task_t three = {
	.name = "three",
};


int main () {
	struct list_head* e;
	struct task_t* t;
	
	list_push_front(&readyQ, &one.q);
	list_push_front(&readyQ, &two.q);
	list_push_front(&readyQ, &three.q);
	
	list_erase(&readyQ,&two.q);
	
	list_push_back(&readyQ, &two.q);
	
	e = readyQ.next;
	while ((e = list_get_front(&readyQ)) != &readyQ) {
		list_erase(&readyQ, e);
		t = container_of(e,struct task_t,q);
		printf("%s\n",t->name);
// 		list_push_back(&readyQ, &t->q);
	}
	return 0;
}*/

