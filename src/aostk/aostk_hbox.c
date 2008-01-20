#include <aostk_hbox.h>
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>
#include <aostk_hbox.h>
#include <list.h>

static void aostk_hbox_draw(struct aostk_widget* w) {
	char first_run = 1;
	struct aostk_hbox* box = (struct aostk_hbox*)w;
// 	int scanlinesy = w->size.width;
// 	int currentx = 0;
	struct list_head* it;
	

	assert(aostk_widget_width(w) >= 0);
	assert(aostk_widget_height(w) >= 0);

	if ((aostk_widget_width(w) == 0) || (aostk_widget_height(w) == 0))
		return;

	aostk_gc_set_fg(&w->gc, 0);
	
	if (box->lines & AOSTK_LINES_TOP)
		aostk_line(w, 0, 0, w->size.width, 0, w->gc.fg_col);
	if (box->lines & AOSTK_LINES_BOTTOM)
		aostk_line(w, 0 , w->size.height, w->size.width, w->size.height, w->gc.fg_col);
	if (box->lines & AOSTK_LINES_LEFT)
		aostk_line(w, 0, 0, 0, w->size.height, w->gc.fg_col);
	if (box->lines & AOSTK_LINES_RIGHT)
		aostk_line(w, w->size.width, 0, w->size.width, w->size.height, w->gc.fg_col);

	list_for_each(it, &box->widget.childlist) {
		struct aostk_widget* content = child_to_widget(it);
		int x;
		
// 		currentx += content->pos.x + content->size.width;
// 		x = aostk_widget_xpos(&box->widget) + currentx;

		x = content->pos.x - box->spacing;// + content->size.width;

// 		if (content->size.width == 0 || content->size.height)
// 			printf("Non visible widget found %s\n", content->name);
		
		content->draw(content);
		if (!first_run && box->lines & AOSTK_LINES_INNER)
			aostk_line(w, x, 0, x, box->widget.size.height - 1, 0x0);

		first_run = 0;
	}
}

static void aostk_hbox_update(struct aostk_widget* w) {
	struct aostk_hbox* box = (struct aostk_hbox*)w;
	struct list_head* it;
	uint8_t translucent = 0;
	struct list_head* list;
	int width=0,height=0;
	int widgetnum = 1;
	int sumwidth  = 0;

	assert(box);

	list = &box->widget.childlist;

	list_for_each(it, list) {
		struct aostk_widget* w = child_to_widget(it);
		if (w->is_translucent)
			translucent = 1;
		
		if (height < w->size.height)
			height = w->size.height;
		
		if (width < w->size.width)
			width = w->size.width;

		w->pos.x = widgetnum * box->spacing + sumwidth;
		w->pos.y =  box->spacing;

		sumwidth += w->size.width;
		widgetnum++;
	}

	if ( (box->widget.is_translucent != translucent) ||
	     (box->widget.size.width != width) ||
	     (box->widget.size.height != height) ) {
		
		box->widget.is_translucent = translucent;
		box->widget.size.width = /*width + box->spacing*2*/sumwidth + widgetnum * box->spacing;
		box->widget.size.height = height + box->spacing*2;
	}
}

void aostk_hbox_push_end(struct aostk_hbox* boxwidget, struct aostk_widget* content) {
// 	struct aostk_hbox* box = (struct aostk_hbox*)boxwidget;
	aostk_widget_reparent(content, AOSTK_WIDGET(boxwidget));
}

void aostk_hbox_init(struct aostk_hbox* box, int x, int y, struct aostk_widget* parent) {
	aostk_widget_init(&box->widget, parent, x, y, 0, 0);
	box->widget.draw = &aostk_hbox_draw;
	box->widget.update = &aostk_hbox_update;
// 	box->spacing = 3;
	box->widget.name = "HBOX";
}

struct aostk_hbox* aostk_hbox_create(int x, int y,  struct aostk_widget* parent) {
	struct aostk_hbox* box = malloc_zero(sizeof(struct aostk_hbox));
	aostk_hbox_init(box, x, y, parent);
	return box;
}
