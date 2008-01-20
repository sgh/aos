#ifndef AOSTK_CHECKBOX
#define AOSTK_CHECKBOX

#include <aostk_widget.h>

struct aostk_checkbox {
	struct aostk_widget widget;
	uint8_t checked;
};

struct aostk_widget* aostk_checkbox_create(uint8_t checked);

#endif
