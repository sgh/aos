/*
	AOS - ARM Operating System
	Copyright (C) 2007  Søren Holm (sgh@sgh.dk)

	License: wxWindows Library Licence, Version 3.1
*/
#include <aos/list.h>
//#include <macros.h>
//#include <task.h>
#include <stdio.h>
//#include <types.h>


void list_push_front(struct list_head *list, struct list_head* node) {
	list->next->prev = node;		// Point front back to new front
	node->next = list->next;		// Point new front to old front
	node->prev = list;					// Point new front back to list_head
	list->next = node;					// Set new front to node
}

void list_push_back(struct list_head* list, struct list_head* node) {
	list->prev->next = node;		// Point back to new back
	node->prev = list->prev;		// Point new back back to old back
	node->next = list;					// Point new back to list_head
	list->prev = node;					// Set new back to node
}



void list_erase(struct list_head* node) {
	/* First we point previous and next entries to each other */
	node->next->prev = node->prev;
	node->prev->next = node->next;

	/* Then we reset the actual entry's next and prev */
// 	node->next = 0;
// 	node->prev = 0;

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
	
// 	list_erase(&readyQ,&two.q);
	
// 	list_push_front(&readyQ, &two.q);
	
	list_for_each(e,&readyQ) {
// 		list_erase(&readyQ, e);
		t = container_of(e,struct task_t,q);
		printf("%s\n",t->name);
// 		list_push_back(&readyQ, &t->q);
	}
	return 0;
}*/

