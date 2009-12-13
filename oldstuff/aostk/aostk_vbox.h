#ifndef AOSTK_VBOX_H
#define AOSTK_VBOX_H

#include <aostk_widget.h>
#include <aostk_boxlayout.h>
#include <list.h>

struct aostk_vbox {
	struct aostk_widget widget;
	unsigned char lines;
// 	struct list_head contents;
};

struct aostk_vbox* aostk_vbox_create(int x, int y,  struct aostk_widget* parent);

void aostk_vbox_init(struct aostk_vbox* box, int x, int y, struct aostk_widget* parent);

void aostk_vbox_push_end(struct aostk_vbox* box, struct aostk_widget* w);

#endif
