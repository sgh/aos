/*
		AOS - ARM Operating System
		Copyright (C) 2007  Søren Holm (sgh@sgh.dk)

		This library is free software; you can redistribute it and/or
		modify it under the terms of the GNU Lesser General Public
		License as published by the Free Software Foundation; either
		version 2.1 of the License, or (at your option) any later version.

		This library is distributed in the hope that it will be useful,
		but WITHOUT ANY WARRANTY; without even the implied warranty of
		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
		Lesser General Public License for more details.

		You should have received a copy of the GNU Lesser General Public
		License along with this library; if not, write to the Free Software
		Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
		*/
#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <stdint.h>

/**
 * \brief Initial value of a linked list.
 */
#define LIST_HEAD_INIT(name) { &(name), &(name) }


/**
 * \brief Macro used for definition of lists.
 * If you fx. wan a linked-list 'mylist'. Do the following:
 * <pre>
 * LIST_HEAD(mylist);
 * </pre>
 */
#define LIST_HEAD(name) \
  struct list_head name = LIST_HEAD_INIT(name)


/**
 * \brief Iterate a list from the start.
 */
#define list_for_each(it,head) \
	for (it = (head)->next; it != (head); it = it->next)


/**
 * \brief Iterate a list from a specific node.
 */
#define list_for_each_from(it,head,from) \
	for (it = (from); it != (head); it = it->next)


/**
 * \brief Double-linked list, or node in such a list.
 */
struct list_head {
	struct list_head *next, *prev;
};


/**
 * \brief Initialize a linked list.
 * @param list The list to initialize.
 */
static inline void INIT_LIST_HEAD(struct list_head *list) {
  list->next = list;
  list->prev = list;
}


/**
 * \brief Append a node to the front of a list.
 * @param list The list to append to.
 * @param node The node to add.
 */
void list_push_front(struct list_head* list, struct list_head* node);


/**
 * \brief Append a node to the back of a list
 * @param list The list to append to.
 * @param node The node to add.
 */
void list_push_back(struct list_head* list, struct list_head* node);


/**
 * \brief Return wether the list is empty.
 * @param list The list in question.
 * @return 0 if the list is not empty.
 */
static inline uint8_t list_isempty(struct list_head* list) {
	return (list->next == list);
}

/**
 * \brief Get the last node in a list.
 * @param list The list to get the last node from. 
 * @return A Pointer to the last node.
 */
static inline struct list_head* list_get_back(struct list_head* list) {
	return  list->prev;
}

/**
 * \brief Get the first node in a list.
 * @param list The list to get the first node from.
 * @return Pointer to the first node.
 */
static inline struct list_head* list_get_front(struct list_head* list) {
	return  list->next;
}


/**
 * \brief Removes a node from the list it belongs.
 * @param node The node to remove.
 */
void list_erase(struct list_head* node);



#endif // _QUEUE_H_
