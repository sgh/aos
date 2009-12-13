#ifndef AOSTK_LABEL_H
#define AOSTK_LABEL_H

#include <aostk_widget.h>

#define AOSTK_LABELSIZE 64

struct aostk_label {
	struct aostk_widget widget;
	char text[AOSTK_LABELSIZE];
	struct aostk_font* font;
};

struct aostk_label* aostk_label_create(const char* text);

void aostk_label_settext(struct aostk_label* w, const char* text);

void aostk_label_init(struct aostk_label* l, const char* text);

#endif
