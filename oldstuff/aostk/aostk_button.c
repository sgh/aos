#include <aostk_button.h>
#include <aostk_font.h>
#include <aostk_widget.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define SPACING 4


static void aostk_button_draw(struct aostk_widget* w) {
	struct aostk_button* b = (struct aostk_button*)w;
	LOG;

	if (aostk_widget_isfocus(w))
		aostk_blockfill(w, 0, 0, w->size.width, w->size.height, 0x000000);
	else
		aostk_blockfill(w, 0, 0, w->size.width, w->size.height, 0x7F7F7F);
		
	
	
	if (aostk_widget_isfocus(w)) {
		aostk_drawframe(w, 0, 0, w->size.width, w->size.height, 0x0084FF);
	}

	if (aostk_widget_isfocus(w))
		aostk_gc_set_fg(&w->gc, 0xFF0000);
	else
		aostk_gc_set_fg(&w->gc, 0xFFFFFF);

	aostk_putstring(w, 0, 0, b->text, aostk_font_height(w->gc.font));
}

static void button_update(struct aostk_button* b) {
	b->widget.size.width = aostk_font_strwidth(&verafont, b->text) + SPACING*2;
	b->widget.size.height = aostk_font_height(&verafont) + SPACING*2;
}


void aostk_button_settext(struct aostk_widget* w, const char* text) {
	struct aostk_button* b = (struct aostk_button*)w;

	if (strcmp(b->text, text) == 0)
		return;
	
	b->text = text;
	button_update(b);
	aostk_widget_repaint(w);
}


static uint8_t button_grab_focus(UNUSED struct aostk_widget* w) {
	return 1;
}


static void aostk_button_activate(struct aostk_widget* w) {
	struct aostk_button* b = (struct aostk_button*)w;
// 	printf("BUTTON ACTIVATED\n");
	if (b->onactivated)
		b->onactivated(&b->widget, b->onactivate_arg);
}

static uint8_t button_input(struct aostk_widget* w, uint32_t ch) {
	uint8_t handled = 0;
	struct aostk_button* l = (struct aostk_button*)w;
// 	int active = l->active;
// 	struct list_head* it;

	LOG;

	switch (ch) {
		case AOSTK_ENTER: {
			aostk_button_activate(w);
			handled = 1;
		}
		break;
	}
	
	aostk_widget_repaint(&l->widget);
	return handled;
}


static void aostk_button_init(struct aostk_button* b, const char* text) {
	b->widget.name = "BUTTON";
	b->widget.draw = &aostk_button_draw;
	b->widget.grab_focus = &button_grab_focus;
	b->widget.activate = &aostk_button_activate;
	b->widget.input = &button_input;
	b->text = text;
	button_update(b);
}


void aostk_button_onactivated(struct aostk_widget* w, void (*onactivated)(struct aostk_widget*, void*), void* arg) {
	struct aostk_button* b = (struct aostk_button*)w;
	b->onactivated = onactivated;
	b->onactivate_arg = arg;
}

struct aostk_widget* aostk_button_create(const char* text) {
	struct aostk_button* b = malloc_zero(sizeof(struct aostk_button));

	aostk_widget_init(&b->widget, NULL, 0, 0, 0, 0);

	aostk_button_init(b, text);

	return &b->widget;
}
