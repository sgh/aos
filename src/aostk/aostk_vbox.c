#include <aostk_vbox.h>
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>
#include <aostk_vbox.h>
#include <list.h>
#include <string.h>

static void aostk_vbox_draw(struct aostk_widget* w) {
	struct aostk_vbox* box = (struct aostk_vbox*)w;
  char first_run = 1;

// 	int scanlinesy = w->size.width;
// 	int currentx = 0;
	struct list_head* it;
	
	assert(aostk_widget_width(w) >= 0);
	assert(aostk_widget_height(w) >= 0);

	if ((aostk_widget_width(w) == 0) || (aostk_widget_height(w) == 0))
		return;

// 	aostk_line(aostk_widget_xpos(w), aostk_widget_ypos(w), aostk_widget_xpos(w) + w->size.width - 1, aostk_widget_ypos(w) + w->size.height - 1, w->gc.fg_col);

	aostk_gc_set_fg(&w->gc, 0);
	//aostk_drawframe(&w->gc, aostk_widget_xpos(w), aostk_widget_ypos(w), w->size.width, w->size.height);

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
		int y;


		y = content->pos.y + content->size.height;

		content->draw(content);

		if (!first_run && box->lines & AOSTK_LINES_INNER)
			aostk_line(w, 0, y, box->widget.size.width - 1, y, 0xF);
		first_run = 0;
	}
}

static void aostk_vbox_update(struct aostk_widget* bw) {
	struct aostk_vbox* box = (struct aostk_vbox*)bw;
	struct list_head* it;
	uint8_t translucent = 0;
	struct list_head* list;
	int width=0, height=0;
	int dynamicnum = 0;
	unsigned int y = 0;
	unsigned int parent_height = 0; // Height of parent
	unsigned int childs_height  = 0; // Height of all fixed-size childs

	assert(box);

	if (bw->parent)
		parent_height = aostk_widget_height(bw->parent);

	list = &box->widget.childlist;

	list_for_each(it, list) {
		struct aostk_widget* w = child_to_widget(it);
		
		if (w->flags & HEIGHT_DYNAMIC)
			dynamicnum++;
		else
			childs_height += w->size.height;
	}

	if (childs_height > parent_height)
		childs_height = parent_height;

	list_for_each(it, list) {
		struct aostk_widget* w = child_to_widget(it);
		if (w->is_translucent)
			translucent = 1;

		if (w->flags & HEIGHT_DYNAMIC)
			w->size.height = (parent_height - childs_height)/dynamicnum;
		
		if (height < w->size.height)
			height = w->size.height;
		
		if (width < w->size.width)
			width = w->size.width;

		w->pos.y = y;
		w->pos.x = 0;

		y += w->size.height;
	}

	if ( (box->widget.is_translucent != translucent) ||
	     (box->widget.size.width != width) ||
	     (box->widget.size.height != height) ) {
		
		box->widget.is_translucent = translucent;
		box->widget.size.height = y;
		box->widget.size.width = width;
	}
}

void aostk_vbox_push_end(struct aostk_vbox* boxwidget, struct aostk_widget* content) {
// 	struct aostk_vbox* box = (struct aostk_vbox*)boxwidget;
	aostk_widget_reparent(content, &boxwidget->widget);
}

void aostk_vbox_init(struct aostk_vbox* box, int x, int y, struct aostk_widget* parent) {
	aostk_widget_init(&box->widget, parent, x, y, 0, 0);
	box->widget.draw = &aostk_vbox_draw;
	box->widget.update = &aostk_vbox_update;
	box->widget.name = "VBOX";
}

struct aostk_vbox* aostk_vbox_create(int x, int y,  struct aostk_widget* parent) {
	struct aostk_vbox* box = malloc_zero(sizeof(struct aostk_vbox));
	memset(box, 0, sizeof(struct aostk_vbox));
	aostk_vbox_init(box, x, y, parent);
	return box;
}
