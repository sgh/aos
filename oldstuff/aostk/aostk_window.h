#ifndef AOSTK_WINDOW_H
#define AOSTK_WINDOW_H

#include <aostk.h>
#include <aostk_widget.h>

struct aostk_window {
	struct aostk_widget widget;
	struct aostk_widget* active;
	unsigned int state;
};

struct aostk_window* aostk_window_create(int x, int y);

void aostk_window_destroy(struct aostk_window* w);

void aostk_window_activate(struct aostk_window* w);

#endif
