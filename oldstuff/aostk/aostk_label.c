#include <aostk_label.h>
#include <aostk_widget.h>
#include <stdlib.h>
#include <string.h>
#include <aostk_font.h>

static void update_dimensions(struct aostk_label* l) {
	l->widget.size.width = aostk_font_strwidth(l->widget.gc.font, l->text);
	l->widget.size.height = aostk_font_height(l->widget.gc.font);
	aostk_widget_update(l->widget.parent);
}

static void aostk_label_draw(struct aostk_widget* w) {
	struct aostk_label* l = (struct aostk_label*)w;
// 	aostk_widget_clear(w);
	aostk_putstring(&l->widget, 0, 0, l->text, aostk_font_height(l->widget.gc.font));
}

void aostk_label_settext(struct aostk_label* w, const char* text) {
	struct aostk_label* l = (struct aostk_label*)w;
	if (strcmp(text, l->text) == 0)
		return;

	strcpy(l->text, text);

	update_dimensions(l);
	aostk_widget_repaint(AOSTK_WIDGET(w));
}

void aostk_label_init(struct aostk_label* l, const char* text) {
	aostk_widget_init(&l->widget, NULL, 0, 0, 0, 0);
	l->widget.draw = &aostk_label_draw;
	memset(l->text, 0, AOSTK_LABELSIZE);
	l->font = &verafont;
	l->widget.is_translucent = 1; // Visual appearence is part glyphs and part parent
	l->widget.name = "LABEL";
	aostk_label_settext(l, text);
	update_dimensions(l);
}

struct aostk_label* aostk_label_create(const char* text) {
	struct aostk_label* l = malloc_zero(sizeof(struct aostk_label));
	aostk_label_init(l, text);
	return l;
}
