#include <aostk_listview.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <assert.h>
#include <aostk_listview.h>
#include <list.h>

static void aostk_listview_draw(struct aostk_widget* w) {
	struct aostk_listview* box = (struct aostk_listview*)w;
// 	int scanlinesy = w->size.width;
// 	int currentx = 0;
	int nr = 0;
	struct list_head* it;
	int sumheight = 0;

	//printf(":::::::::::::::::::::::::::::::::::::: %d %d\n", aostk_widget_width(w), aostk_widget_width(w));

	assert(aostk_widget_width(w) >= 0);
	assert(aostk_widget_height(w) >= 0);

	if ((aostk_widget_width(w) == 0) || (aostk_widget_height(w) == 0))
		return;

	list_for_each(it, &box->widget.childlist) {
		struct aostk_widget* content = child_to_widget(it);
		int y;

		nr ++;
		
// 		currentx += content->pos.x + content->size.width;
// 		x = aostk_widget_xpos(&box->widget) + currentx;

// 		x = aostk_widget_xpos(&box->widget) + content->pos.x + content->size.width;
		y = /*aostk_widget_ypos(&box->widget) +*/ content->pos.y + content->size.height;

		if (nr == box->active ) {
			if (aostk_widget_isfocus(box))
				aostk_blockfill(w, 0, sumheight + nr*box->spacing, box->widget.size.width, content->size.height, 0x007FFF);
			else
				aostk_blockfill(w, 0, sumheight + nr*box->spacing, box->widget.size.width, content->size.height, 0x7f7f7f);

			
		}

			

		sumheight += content->size.height;
		
// 		if (content->size.width == 0 || content->size.height)
// 			printf("Non visible widget found %s\n", content->name);
		
		content->draw(content);
// 		aostk_line(x, aostk_widget_ypos(&box->widget), x, aostk_widget_ypos(&box->widget) + box->widget.size.height - 1, 0xF);
		aostk_line(w, 0, y, box->widget.size.width - 1, y, 0xF);
	}

	aostk_drawframe(w, 0, 0, w->size.width, w->size.height, 0x0);

}

static void aostk_listview_update(struct aostk_widget* w) {
	struct aostk_listview* box = (struct aostk_listview*)w;
	struct list_head* it;
	uint8_t translucent = 0;
	struct list_head* list;
	int width=0,height=0;
	int widgetnum = 1;
// 	int sumwidth  = 0;
	int sumheight  = 0;

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

		w->pos.y = widgetnum * box->spacing + sumheight;
		w->pos.x =  box->spacing;

// 		sumwidth += w->size.width;
		sumheight += w->size.height;
		widgetnum++;
	}

	box->numwidgets = widgetnum;

	if ( (box->widget.is_translucent != translucent) ||
	     (box->widget.size.width != width) ||
	     (box->widget.size.height != height) ) {
		
		box->widget.is_translucent = translucent;
		box->widget.size.height = sumheight + widgetnum * box->spacing;
		box->widget.size.width = width + box->spacing*2;
	}
}

void aostk_listview_push_start(struct aostk_listview* lv, struct aostk_widget* content, void* arg) {
// 	struct aostk_listview* box = (struct aostk_listview*)boxwidget;
	content->scratchpad = arg;
	aostk_widget_reparent(content, &lv->widget);
// 	aostk_redraw(boxwidget);
}

static uint8_t listview_grab_focus(UNUSED struct aostk_widget* w) {
	return 1;
}


static void aostk_listview_activate(struct aostk_widget* w) {
	struct aostk_listview* l = (struct aostk_listview*)w;
	int active = l->active;
	struct list_head* it;
	int i = 1;

	LOG;

	if (!l->onactivated)
		return;

	list_for_each(it, &l->widget.childlist) {
		if (i == active) {
			l->onactivated(l, (uint32_t)child_to_widget(it)->scratchpad);
			break;
		}
		i++;
	}
}

static uint8_t listview_input(struct aostk_widget* w, uint32_t ch) {
	uint8_t handled = 0;
	struct aostk_listview* l = (struct aostk_listview*)w;
	int active = l->active;
// 	struct list_head* it;

	switch (ch) {
		case AOSTK_UP:
			if (active > 1 )
				active--;
			handled = 1;
			break;
		case AOSTK_DOWN:
			if (active < l->numwidgets-1)
				active++;
			handled = 1;
			break;
		case AOSTK_ENTER: {
			aostk_listview_activate(&l->widget);
			handled = 1;
		}
		break;
	}

	l->active = active;
	aostk_widget_repaint(&l->widget);
	return handled;
}

void aostk_listview_onactivated(struct aostk_listview* lv, void (*onactivated)(struct aostk_listview* l, int nr), UNUSED void* arg) {
	lv->onactivated = onactivated;
// 	b->onactivate_arg = arg;
}

void aostk_listview_init(struct aostk_listview* box, int x, int y, struct aostk_widget* parent) {
	aostk_widget_init(&box->widget, parent, x, y, 0, 0);
	box->widget.draw = &aostk_listview_draw;
	box->widget.update = &aostk_listview_update;
	box->spacing = 3;
	box->widget.name = "LVIEW";
	box->widget.grab_focus = &listview_grab_focus;
	box->widget.activate = &aostk_listview_activate;
	box->widget.input = &listview_input;
	box->active = 0;
}

struct aostk_listview* aostk_listview_create(int x, int y,  struct aostk_widget* parent) {
	struct aostk_listview* box = malloc_zero(sizeof(struct aostk_listview));
	aostk_listview_init(box, x, y, parent);
	return box;
}
