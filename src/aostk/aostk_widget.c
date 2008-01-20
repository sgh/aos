// #include <aostk.h>
#include <aostk_widget.h>
#include <aostk_window.h>
#include <aostk_font.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <aostk_fontrenderer.h>
#include <aostk_widget.h>

struct aostk_widget* active_widget = NULL;
struct aostk_window* active_window = NULL;

static void aostk_widget_draw(struct aostk_widget* w) {
	aostk_widget_clear(w);
}

static void aostk_widget_activate(UNUSED struct aostk_widget* w) {
}

void aostk_widget_clear(struct aostk_widget* w) {
	LOG;

	if (w->is_translucent)
		return;

	aostk_blockfill(w, 0, 0, aostk_widget_width(w), aostk_widget_height(w), w->gc.bg_col);
}


void aostk_widget_repaint(struct aostk_widget* w) {
	assert(w);

// 	if (!w->parent)
// 		return;
	
// 	w->events |= AOSTK_REFRESH;

	if (w->is_translucent) {
		if (w->parent)
			aostk_widget_repaint(w->parent);
// 		else
// 			aostk_redraw(NULL);
	}
// 		else
// 		aostk_redraw(w);
}


void aostk_widget_reparent(struct aostk_widget* w, struct aostk_widget* parent) {

	if (w->parent)
		list_erase(&w->childnode);

// 	if (w->parent)
// 		aostk_redraw(w->parent);
	
	w->parent = parent;
	list_push_back(&parent->childlist, &w->childnode);
	aostk_widget_update(parent);

	aostk_widget_repaint(w);

}

void aostk_widget_set_fixed(struct aostk_widget* w, int width, int height) {
	w->is_size_fixed = 1;
	w->size.width = width;
	w->size.height = height;
}

void aostk_widget_init(struct aostk_widget* w, struct aostk_widget* parent, int x, int y, int width, int height) {
	w->parent = parent;
	w->draw = &aostk_widget_draw;
	w->activate = &aostk_widget_activate;
	INIT_LIST_HEAD(&w->childlist);
	INIT_LIST_HEAD(&w->childnode);
	INIT_LIST_HEAD(&w->widgetnode);

	w->size.width = width;
	w->size.height = height;

	w->pos.x = x;
	w->pos.y = y;

	w->name = "WIDGET";

	// Place in end in global widget-list
	list_push_back(&aostk_widgetlist, &w->widgetnode);

	// If widget has a parent, add the widget in the end for the parents child-list
	if (w->parent)
		list_push_back(&parent->childlist, &w->childnode);

	// Initialize to default context
	w->gc.bg_col = 0xFFFFFF;
	w->gc.fg_col = 0x000000;
	w->gc.font = &verafont;
}

struct aostk_widget* aostk_widget_create(int x, int y, int width, int height, struct aostk_widget* parent) {
	struct aostk_widget* w = malloc_zero(sizeof(struct aostk_widget));
	aostk_widget_init(w, parent, x, y, width, height);
	w->size.height = height;
	w->size.width = width;
	return w;
}


int aostk_widget_xpos(struct aostk_widget* w) {
	int x = 0;
	/** @todo this function must be optimixed to be O(1) */
	if (w->parent)
		x += aostk_widget_xpos(w->parent);
	x += w->pos.x;
	return x;
}

int aostk_widget_ypos(struct aostk_widget* w) {
	int y = 0;
	/** @todo this function must be optimixed to be O(1) */
	if (w->parent)
		y += aostk_widget_ypos(w->parent);
	y += w->pos.y;
	return y;
}

uint8_t aostk_grab_focus(struct aostk_widget* w) {
	if (w && w->grab_focus && w->grab_focus(w)) {
		if (active_widget) {
// 			active_widget->is_focus = 0;
			if (w->ungrab_focus)
				w->ungrab_focus(w);
		}

// 		w->is_focus = 1;

		// First clear old focus, then draw new
		if (active_widget)
			aostk_widget_repaint(active_widget);
		aostk_widget_repaint(w);
		active_widget = w;
		return 1;
	}
	return 0;
}

static uint8_t focus_valid(struct aostk_widget* w) {
	struct aostk_widget* toplevel = w;

	assert(w);

	if (!w->grab_focus)
		return 0;

	while (toplevel->parent)
		toplevel = toplevel->parent;

	if (toplevel == AOSTK_WIDGET(active_window))
		return 1;

	return 0;
}


void aostk_activate(struct aostk_widget* w) {
// 	assert(w);
	
	if (w == NULL)
		w = active_widget;

	if (w)
		w->activate(w);
}


struct aostk_widget* aostk_focus_next() {
//return NULL; /** @FIXME this destroys operartion for some reason */
	struct list_head* it;
	struct aostk_widget* next = NULL;
	struct aostk_widget* w;
	struct list_head* from;

	/** @todo port to use find_surrounding_widgets */
	
	if (/*!active_widget ||*/ !active_window)
		return NULL;

	if (active_widget) {
// 		printf("Trying from the active widget\n");
		from = &active_widget->widgetnode;//list_get_front(&active_widget->widgetnode);
	} else {
// 		printf("Trying from the beginning\n");
		from = list_get_front(&aostk_widgetlist);
	}

	list_for_each_from(it, &aostk_widgetlist, from) {
  	w = container_of(it, struct aostk_widget, widgetnode);
// 		printf("%s\n", w->name);

		// Stop when reaching current focus
		if (w == active_widget)
			break;

		// Now if widget is not a valid focus - continue
		if (!focus_valid(w))
			continue;
		
		if (aostk_grab_focus(w)) {
			next = w;
			break;
		}
	}

	return NULL;

	if (!next) {
// 		printf("Trying from the beginning\n");
		list_for_each(it, &aostk_widgetlist) {
			w = container_of(it, struct aostk_widget, widgetnode);
// 			printf("%s\n", w->name);

			// Stop when reaching current focus
			if (w == active_widget)
				break;

			// Now if widget is not a valid focus - continue
			if (!focus_valid(w))
				continue;
		
			if (aostk_grab_focus(w)) {
				next = w;
				break;
			}
		}
	}

// 	list_for_each(it, &aostk_widgetlist) {
// 		w = container_of(it, struct aostk_widget, widgetnode);
// 		printf("%s\n", w->name);
// 	}
	
	return next;
}

static void find_surrounding_widgets(struct aostk_widget** left, struct aostk_widget** right)  {
	struct list_head* it;
// 	struct aostk_widget* next = NULL;
	struct aostk_point left_placement;
	struct aostk_point right_placement;
	struct aostk_point tmp_placement;
	struct aostk_widget* w;

	struct aostk_point active_placement;

	*left = NULL;
	*right = NULL;

	if (!active_widget || !active_window)
		return;

	active_placement.x = aostk_widget_xpos(active_widget) + (aostk_widget_width(active_widget)>>1);
	active_placement.y = aostk_widget_ypos(active_widget) + (aostk_widget_height(active_widget)>>1);

	left_placement.x = 0;
	left_placement.y = 0;

	right_placement.x = 0xFFFFFFFF;
	right_placement.y = 0xFFFFFFFF;

// 		printf("Trying from the beginning\n");
// 	from = list_get_front(&aostk_widgetlist);
	
	list_for_each(it, &aostk_widgetlist) {
  	w = container_of(it, struct aostk_widget, widgetnode);
// 		printf("%s\n", w->name);


		// Now if widget is not a valid focus - continue
		if (!focus_valid(w))
			continue;

		tmp_placement.x = aostk_widget_xpos(w) + (aostk_widget_width(w)>>1);
		tmp_placement.y = aostk_widget_ypos(w) + (aostk_widget_height(w)>>1);

		if ((tmp_placement.x > left_placement.x) && (tmp_placement.x < active_placement.x)) {
			left_placement = tmp_placement;
			*left = w;
		}

		if ((tmp_placement.x < right_placement.x) && (tmp_placement.x > active_placement.x)) {
			right_placement = tmp_placement;
			*right = w;
		}

	}
}

struct aostk_widget* aostk_focus_left(void) {
	struct aostk_widget* left;
	struct aostk_widget* right;

	find_surrounding_widgets(&left, &right);

	if (left)
		aostk_grab_focus(left);

	return left;
}

struct aostk_widget* aostk_focus_right(void) {
	struct aostk_widget* left;
	struct aostk_widget* right;

	find_surrounding_widgets(&left, &right);

	if (right)
		aostk_grab_focus(right);
	return right;
}


void aostk_widget_update(struct aostk_widget* w) {
	LOG;

	if (w) {
		if (w->update)
			w->update(w);
		if (w->parent && w->parent->update)
			w->parent->update(w->parent);
	}
}


void aostk_drawframe(struct aostk_widget* w,  int x, int y, int width, int height, uint32_t color) {
	assert(width > 0);
	assert(height > 0);

	width -= 1;
	height -= 1;

	aostk_line(w, x, y, x + width, y, color);
	aostk_line(w, x + width, y, x + width, y + height, color);
	aostk_line(w, x, y + height, x + width, y + height, color);
	aostk_line(w, x, y, x, y + height, color);
}

void aostk_putstring(struct aostk_widget* w, int x, int y, const char* str, int scanlines) {
	struct aostk_font* f = w->gc.font;
	const struct aostk_glyph* g;
	assert(f != NULL);
	/**
	 * Y-position is default not the baseline, but the topmost pixel of the font
	 * So calculate the baseline by adding the font height
	 */
	y += f->height;
	

	while ((*str) != 0) {
		g = aostk_get_glyph(f, (uint8_t)*str);
		aostk_putglyph(g, x, y, (uint8_t)*str, scanlines, w);
		x += g->advance.x;
		str++;
	}
}


