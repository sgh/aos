#include <aostk_spinbox.h>
#include <aostk_widget.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <aostk_font.h>
#include <aostk_window.h>

#define SPACING 4


static void aostk_spinbox_draw(struct aostk_widget* w) {
	char buffer[16];
	struct aostk_spinbox* s = (struct aostk_spinbox*)w;

	aostk_widget_clear(w);

	if (aostk_widget_isfocus(w)) {
		aostk_drawframe(w, 1, 1, w->size.width-2, w->size.height-2, 0x7F7F7F);
  	aostk_drawframe(w, 0, 0, w->size.width, w->size.height, 0x0084FF);
	} else
		aostk_drawframe(w, 0, 0, w->size.width, w->size.height, 0x7F7F7F);

	sprintf(buffer,"%4d", s->value);
	aostk_putstring(w, 0, 1, buffer, 0); /** @todo fixme */
}

void aostk_spinbox_setvalue(struct aostk_widget* w, int value) {
	struct aostk_spinbox* s = (struct aostk_spinbox*)w;
	uint8_t update = 0;

	if (s->value != value)
		update = 1;

	s->value = value;

// 	if (update)
// 		aostk_redraw(w);
}

static void spinbox_update(struct aostk_spinbox* s) {
	char buffer[16];
	sprintf(buffer,"%4d",s->value);
	s->widget.size.width = aostk_font_strwidth(s->widget.gc.font, buffer) + SPACING;
	s->widget.size.height = verafont.height + SPACING;
}

static uint8_t spinbox_grab_focus(UNUSED struct aostk_widget* w) {
	return 1;
}


static uint8_t spinbox_input(struct aostk_widget* w, uint32_t ch) {
	uint8_t handled = 0;
	struct aostk_spinbox* s = (struct aostk_spinbox*)w;
	int value = s->value;
// 	struct list_head* it;

	switch (ch) {
		case AOSTK_UP:
			value++;
			handled = 1;
			break;
		case AOSTK_DOWN:
			value--;
			handled = 1;
			break;
		case AOSTK_ENTER:
			aostk_grab_focus(s->last_focus);
			s->last_focus = NULL;
			handled = 1;
			break;
	}

	s->value = value;
	aostk_widget_repaint(&s->widget);
	return handled;
}


static void aostk_spinbox_activate(struct aostk_widget* w) {
	struct aostk_spinbox* s = (struct aostk_spinbox*)w;
	s->last_focus = active_widget;
// 	printf("###############################################################");
	aostk_grab_focus(w);
// 	struct aostk_spinbox* b = (struct aostk_spinbox*)w;
// 	printf("BUTTON ACTIVATED\n");
// 	if (b->onactivated)
// 		b->onactivated(b, b->onactivate_arg);
}

static void aostk_spinbox_init(struct aostk_spinbox* s, int value) {
	s->widget.name = "SPINBOX";
	s->widget.draw = &aostk_spinbox_draw;
	s->widget.grab_focus = &spinbox_grab_focus;
	s->widget.activate = &aostk_spinbox_activate;
	s->widget.input = &spinbox_input;
	s->value = value;
	spinbox_update(s);
}


void aostk_spinbox_onactivated(struct aostk_widget* w, void (*onactivated)(struct aostk_spinbox* button, void* arg), void* arg) {
	struct aostk_spinbox* b = (struct aostk_spinbox*)w;
	b->onactivated = onactivated;
	b->onactivate_arg = arg;
}

struct aostk_widget* aostk_spinbox_create(int value) {
	struct aostk_spinbox* s = malloc_zero(sizeof(struct aostk_spinbox));
// 	s->label = (struct aostk_label*)aostk_label_create(&testfont, text);
	
	aostk_widget_init(&s->widget, NULL, 0, 0, 0, 0);

	aostk_spinbox_init(s, value);

// 	aostk_widget_reparent(&b->label->widget, &b->widget);
	
// 	aostk_label_init(&b->label, &testfont, text);
// 	aostk_widget_init(&b->widget, NULL, 0, 0, b->label.widget.size.width, b->label.widget.size.height);

	
	return &s->widget;
}
