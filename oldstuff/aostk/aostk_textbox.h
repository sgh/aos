#ifndef AOSTK_TEXTBOX_H
#define AOSTK_TEXTBOX_H

#include <aostk_widget.h>

struct aostk_textbox {
	struct aostk_widget widget;
	const char* text;
};

struct aostk_textbox* aostk_textbox_create(const char* text);

void aostk_textbox_settext(struct aostk_textbox* w, const char* text);

void aostk_textbox_init(struct aostk_textbox* l, const char* text);

#endif
