#ifndef AOSTK_BUTTON_H
#define AOSTK_BUTTON_H

#include <aostk_widget.h>

struct aostk_button {
	struct aostk_widget widget;
	const char* text;
	void (*onactivated)(struct aostk_widget* button, void* arg);
	void* onactivate_arg;
};

struct aostk_widget* aostk_button_create(const char* text);

void aostk_button_settext(struct aostk_widget* w, const char* text);

void aostk_button_onactivated(struct aostk_widget* w, void (*onactivated)(struct aostk_widget*, void*), void* arg);

#endif
