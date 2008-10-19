#ifndef DRAWABLE_H
#define DRAWABLE_H

#include "gtkfb.h"
#include <stdint.h>

struct Point {
	int x;
	int y;
};

struct DrawingContext {
	unsigned int fg_color;
	unsigned int bg_color;
	unsigned int text_color;
	bool alphablend;
};

struct Box {
	int x;
	int y;
	int width;
	int height;
};

class DrawableDecoration {
		
	public:
		int _top;
		int _bottom;
		int _left;
		int _right;
		
		DrawableDecoration(void) {}

		virtual void draw(class Drawable* d) {};
};

extern class SimpleLineDecoration testdecoration;

class Drawable {

	Drawable* _parent;   // Parent Drawable
	Drawable* _children; // list of child drawables

	Drawable* _next; // Next Drawable is the parents child list
	Drawable* _prev; // Previous Drawable is the parents child list
	unsigned int _events;

	void invalidateOverlapped(void);
	void postEvent(unsigned int event);

protected:
	bool             _visible;
	bool             _transparent;
	bool             _modal;

	void update(void);
	virtual void erase(void);
	uint32_t _last_update;

	// Decoration aware functions
	int absx_deco(void);
	int absy_deco(void);
	int width_deco(void);
	int height_deco(void);

	// Real handlers for events
	void real_invalidate(void);
	void real_show();
	void real_hide();

public:
	struct Point     _abs_xy; // Absolute position
	struct Point     _rel_xy; // Releative position
	int _width;
	int _height;

	class DrawableDecoration* _decoration;

	struct DrawingContext _ctx;

	bool _dirty;

	Drawable(int x, int y, int width, int height);

	bool redraw(void);

	virtual void predraw(void) {};

	virtual void draw(void) {};

	bool isVisible(void);

	void addChild(Drawable& child);

	void removeChild(Drawable& child);

	void setXY(int x, int y);

	void setBG(int col);

	void setFG(int col);

	/**
	 * \brief Invalidate the drawable and scheduele it to be redrawn
	 */
	void invalidate(void);
	void show();
	void hide();

	void invalidate_elapsed(int ms);

	void processEvents(void);

	void focus(void);
	virtual void focus_in(void) {};
	virtual void focus_out(void) {};

	virtual void key_event(struct extended_char* xchar) {}

	int width(void)  { return _width;  }
	int height(void) { return _height; }

	Drawable* next(void) { return _next; }
	Drawable* prev(void) { return _prev; }

	struct Box intersection(Drawable& d);

	friend class UGui;
};


#endif
