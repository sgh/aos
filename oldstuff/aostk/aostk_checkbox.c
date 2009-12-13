#include <aostk.h>
#include <aostk_checkbox.h>
#include <stdlib.h>

static void aostk_checkbox_draw(struct aostk_widget* w) {
	struct aostk_checkbox* cb = (struct aostk_checkbox*)w;
	
	aostk_drawframe(w, 0, 0, w->size.width, w->size.height, 0x0);

	aostk_drawframe(w, 1, 1, w->size.width-2, w->size.height-2, 0xcccccc);

	if (cb->checked) {
		aostk_line(w, 1, 1, w->size.width-2 , w->size.height-2, 0x00FF00);
		aostk_line(w, w->size.width-2, 1, 1, w->size.height-2, 0x00FF00);
// 			aostk_drawbox(aostk_widget_xpos(w)+1, aostk_widget_ypos(w)+1, w->size.width-1, w->size.height-1, 0);
	}
}

static void aostk_checkbox_activate(struct aostk_widget* w) {
	struct aostk_checkbox* cb = (struct aostk_checkbox*)w;
	cb->checked ^= 1;
	aostk_widget_repaint(w);
}

static void aostk_checkbox_init(struct aostk_checkbox* cb, uint8_t checked) {
	cb->checked = checked;
	cb->widget.draw = &aostk_checkbox_draw;
	cb->widget.activate = &aostk_checkbox_activate;
	cb->widget.name = "CHECKBOX";
}

struct aostk_widget* aostk_checkbox_create(uint8_t checked) {
	struct aostk_checkbox* cb = malloc_zero(sizeof(struct aostk_checkbox));
	
	aostk_widget_init(&cb->widget, NULL, 0, 0, 14, 14);

	aostk_checkbox_init(cb, checked);
	
	return &cb->widget;
}
