#ifndef AOSTK_SPINBOX_H
#define AOSTK_SPINBOX_H

#include <aostk_label.h>

struct aostk_spinbox {
	struct aostk_widget widget;
	struct aostk_widget* last_focus;
	int value;
	void (*onactivated)(struct aostk_spinbox* spinbox, void* arg);
	void* onactivate_arg;
};

struct aostk_widget* aostk_spinbox_create(int value);

void aostk_spinbox_setvalue(struct aostk_widget* w, int value);

void aostk_spinbox_onactivated(struct aostk_widget* w, void (*onactivated)(struct aostk_spinbox* b, void* arg), void* arg);

#endif
