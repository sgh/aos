#ifndef AOSTK_WIDGET_H
#define AOSTK_WIDGET_H

#include <stddef.h>

#include <aostk.h>
#include <aostk_context.h>

#define child_to_widget(it) container_of(it,struct aostk_widget, childnode)
// #define to_aostk_widget(it) container_of(it,struct aostk_widget, childnode)

#define AOSTK_WIDGET(w) (&((w)->widget))

struct aostk_window;

extern struct aostk_window* active_window;
extern struct aostk_widget* active_widget;

#define HEIGHT_DYNAMIC BIT0

struct aostk_widget {
	const char* name;
	uint8_t is_translucent:1;
	uint8_t is_window:1;
	uint8_t is_size_fixed:1;
	uint8_t flags;

	/**
	 * \brief Repaint the widget
	 */
	void (*draw)(struct aostk_widget*);

	/**
	 * \brief Functon that updates information of children data
	 */
	void (*update)(struct aostk_widget*);

	/**
	 * \brief Instruct widget to grab focus
	 * @return True if the widget grabed focus
	 */
	uint8_t (*grab_focus)(struct aostk_widget*);

	/**
	 * \brief Called to ungrab focus from the widget
	 */
	uint8_t (*ungrab_focus)(struct aostk_widget*);

	/**
	 * \brief Called to activate the widget. Fx.at [enter]
	 */
	void (*activate)(struct aostk_widget*);

	/**
	 * \brief Called to push keyboard input to widget
	 * @return 1 if the key is handdled - 0 if not
	 */
	uint8_t (*input)(struct aostk_widget*, uint32_t ch);
	
	struct aostk_point pos;
	struct aostk_size  size;
// 	struct aostk_size  margin;
	struct aostk_gc    gc;
	void* scratchpad;


	// Node in parents child_list
	struct list_head childnode;

	// Node in wodget list
	struct list_head widgetnode;

	struct aostk_widget* parent;
	
	// Lists
	struct list_head childlist;
};

// Events
#define AOSTK_UPDATE_DIM     (1<<0)
#define AOSTK_UPDATE_CONTENT (1<<1)
#define AOSTK_SHOW           (1<<2)
#define AOSTK_HIDE           (1<<3)
#define AOSTK_REFRESH        (1<<4)

// States
#define AOSTK_VISIBLE     (1<<0)

// Flags
#define AOSTK_CAN_FOCUS (1<<0)

void aostk_widget_init(struct aostk_widget* w, struct aostk_widget* parent, int x, int y, int width, int height);

struct aostk_widget* aostk_widget_create(int x, int y, int width, int height, struct aostk_widget* parent);

void aostk_widget_init(struct aostk_widget* w, struct aostk_widget* parent, int x, int y, int width, int height);

static inline unsigned char aostk_widget_isfocus(struct aostk_widget* w) {
	return (active_widget == w);
}

// void aostk_widget_show(struct aostk_widget* w);

// void aostk_widget_hide(struct aostk_widget* w);

void aostk_widget_repaint(struct aostk_widget* w);

void aostk_widget_clear(struct aostk_widget* w);

int aostk_widget_xpos(struct aostk_widget* w);

int aostk_widget_ypos(struct aostk_widget* w);

static inline int aostk_widget_width(struct aostk_widget* w) {
	return w->size.width /*+ w->margin.width*2*/;
}

static inline int aostk_widget_height(struct aostk_widget* w) {
	return w->size.height /*+ w->margin.height*2*/;
}

uint8_t aostk_grab_focus(struct aostk_widget* w);

void aostk_widget_update(struct aostk_widget* w);

void aostk_widget_reparent(struct aostk_widget* w, struct aostk_widget* parent);

struct aostk_widget* aostk_focus_next(void);

struct aostk_widget* aostk_focus_left(void);

struct aostk_widget* aostk_focus_right(void);

void aostk_activate(struct aostk_widget* w);

void aostk_widget_set_fixed(struct aostk_widget* w, int width, int height);

static inline void aostk_line(struct aostk_widget* w, int x1, int y1, int x2, int y2, uint32_t color) {
	current_device->line(aostk_widget_xpos(w) + x1, aostk_widget_ypos(w) + y1, aostk_widget_xpos(w) + x2, aostk_widget_ypos(w) + y2, color);
}

static inline void aostk_putpixel(struct aostk_widget* w, int x, int y, uint32_t color) {
	current_device->putpixel(aostk_widget_xpos(w) + x, aostk_widget_ypos(w) + y, color);
}

static inline void aostk_blockfill(struct aostk_widget* w, int x, int y, int width, int height, uint32_t color) {
	current_device->blockfill(aostk_widget_xpos(w) + x, aostk_widget_ypos(w) + y, width, height, color);
}

static inline void aostk_rawgrayline(struct aostk_widget* w, int x, int y, void* src, int n) {
	if (current_device->rawgrayline)
		current_device->rawgrayline(aostk_widget_xpos(w) + x, aostk_widget_ypos(w) + y, src, n);
}

void aostk_putstring(struct aostk_widget* w, int x, int y, const char* str, int scanlines);

#endif
