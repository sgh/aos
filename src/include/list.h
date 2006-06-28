#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <types.h>


#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define LIST_HEAD(name) \
  struct list_head name = LIST_HEAD_INIT(name)

#define list_for_each(it,head) \
	for (it = (head)->next; it != (head); it = it->next)
	
#define list_for_each_from(it,head,from) \
	for (it = (from); it != (head); it = it->next)

extern struct list_head readyQ;
extern struct list_head msleepQ;
extern struct list_head usleepQ;

struct list_head {
	struct list_head *next, *prev;
};


static inline void INIT_LIST_HEAD(struct list_head *list) {
  list->next = list;
  list->prev = list;
}


void list_push_front(struct list_head* q, struct list_head* qe);
void list_push_back(struct list_head* q, struct list_head* qe);
struct list_head* list_get_back(struct list_head* qe);
struct list_head* list_get_front(struct list_head* q);
void list_erase(struct list_head* q, struct list_head* qe);
uint8 list_isempty(struct list_head* q);

extern struct list_head readyQ;




#endif // _QUEUE_H_
