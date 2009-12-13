#include <aostk_widget.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <aostk_window.h>
#include <aostk_window.h>
#define SPACING 4

int yoffset = 0;

static void aostk_window_draw(struct aostk_widget* w) {
	struct aostk_window* win = (struct aostk_window*)w;
	struct list_head* it;

// 	w->pos.y += yoffset; // test to scroll window

// 	win->state ^= 1;
// 
// 	if (win->state)
// 		aostk_gc_set_bg(&w->gc, 0x00FFFF);
// 	else
// 		aostk_gc_set_bg(&w->gc, 0x007F7F);

	aostk_widget_clear(w);
	list_for_each(it, &w->childlist) {
		struct aostk_widget* w = child_to_widget(it);
		w->draw(w);
	}

	if (w == (struct aostk_widget*)active_window)
		aostk_drawframe(w, 0, 0, aostk_widget_width(w), aostk_widget_height(w), 0xFF00FF);
	else
		aostk_drawframe(w, 0, 0, aostk_widget_width(w), aostk_widget_height(w), 0x0000FF);

	aostk_line(w, 0, 0, w->size.width - 1, w->size.height - 1, 0xFFFF00);
	aostk_line(w, 0, w->size.height - 1, w->size.width - 1, 0, 0xFFFF00);

// 	w->pos.y -= yoffset;  // test to scroll window
}

static void aostk_window_update(struct aostk_widget* window) {
	struct list_head* it;
	struct list_head* list;
	int width=0,height=0;

	assert(window);

	list = &window->childlist;
	
	list_for_each(it, list) {
		struct aostk_widget* w = child_to_widget(it);

		if (height < w->size.height)
			height = w->size.height + w->pos.y;

		if (width < w->size.width)
			width = w->size.width + w->pos.x;
	}

	if (!window->is_size_fixed) {
		window->size.width = width;
		window->size.height = height;
	}

	if (active_widget == NULL)
		aostk_focus_next();
}

static void aostk_window_init(struct aostk_window* w) {
	w->widget.name = "WINDOW";
	w->widget.draw = &aostk_window_draw;
	w->widget.update = &aostk_window_update;
// 	w->widget.margin.width = 3;
// 	w->widget.margin.height = 3;
	w->widget.is_window = 1;
}


static void window_detach(struct aostk_window* win) {
	assert(win);

	if (win == active_window) {
		active_widget = NULL;
		active_window = NULL;
	}

	list_erase(&win->widget.childnode);

	win = (struct aostk_window*)child_to_widget(list_get_back(&aostk_windowlist));
	aostk_window_activate(win);
}


void aostk_window_activate(struct aostk_window* win) {
	
	assert(win);

	active_window = win;

	// Get the currently focused widget in the window
	active_widget = win->active;

	// If no widget is focused, try to focues one
	if (!active_widget)
		aostk_focus_next();
	
// 	aostk_widget_repaint(&win->widget);
}


static void window_attach(struct aostk_window* win) {
	assert(win);

	// Store active widget if a window has focus
	if (active_window)
		active_window->active = active_widget;
	
	// Make sure window is at top
	list_erase(&win->widget.childnode);
	list_push_back(&aostk_windowlist, &win->widget.childnode);
}

static void aostk_window_add(struct aostk_window* w) {
	window_attach(w);
	aostk_window_activate(w);
}


struct aostk_window* aostk_window_create(int x, int y) {
	struct aostk_window* w = malloc_zero(sizeof(struct aostk_window));
	aostk_widget_init(&w->widget, NULL, x, y, 0, 0);
	aostk_window_init(w);
	aostk_window_add(w);
	return w;
}

void aostk_window_destroy(struct aostk_window* win) {
	window_detach(win);
	aostk_free(win);
// 	aostk_free(AOSTK_WIDGET(win));
}
