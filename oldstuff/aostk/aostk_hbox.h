#ifndef AOSTK_HBOX_H
#define AOSTK_HBOX_H

#include <aostk_widget.h>
#include <aostk_boxlayout.h>
#include <list.h>
#include <bits.h>

struct aostk_hbox {
	struct aostk_widget widget;
	int spacing;
	unsigned char lines;
};

struct aostk_hbox* aostk_hbox_create(int x, int y,  struct aostk_widget* parent);

void aostk_hbox_init(struct aostk_hbox* box, int x, int y, struct aostk_widget* parent);

void aostk_hbox_push_end(struct aostk_hbox* box, struct aostk_widget* w);

#endif
