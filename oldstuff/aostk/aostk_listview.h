#ifndef AOSTK_LISTVIEW_H
#define AOSTK_LISTVIEW_H

#include <aostk_widget.h>
#include <list.h>

struct aostk_listview {
	struct aostk_widget widget;
	void (*onactivated)(struct aostk_listview* l, int nr);
// 	void* onactivate_arg;
	int spacing;
	int active;
	int numwidgets;
};

struct aostk_listview* aostk_listview_create(int x, int y,  struct aostk_widget* parent);

void aostk_listview_init(struct aostk_listview* box, int x, int y, struct aostk_widget* parent);

void aostk_listview_push_start(struct aostk_listview* lv, struct aostk_widget* w, void* arg);

void aostk_listview_onactivated(struct aostk_listview* w, void (*onactivated)(struct aostk_listview* lv, int nr), void* arg);

#endif
