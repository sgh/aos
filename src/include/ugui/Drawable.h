#ifndef UGUI_DRAWABLE_H
#define UGUI_DRAWABLE_H

#include "gtkfb.h"
#include <stdint.h>
#include <string.h>
#include <aos/macros.h>
#include <aos/input.h>

struct aostk_font;

typedef enum {UGUI_VERTICAL, UGUI_HORIZONTAL} ugui_orientation_e;

struct Point {
	int x;
	int y;
};

struct DrawingContext {
	unsigned int fg_color;
	unsigned int bg_color;
	unsigned int text_color;
	unsigned int text_outline;
	bool alphablend;
};

struct Box {
	int x;
	int y;
	int width;
	int height;
};

#ifdef __cplusplus

class DrawableDecoration {
		
	public:
		int _top;
		int _bottom;
		int _left;
		int _right;
		struct DrawingContext _ctx;
		
		DrawableDecoration(void) { memset(&_ctx, 0, sizeof(_ctx)); }

		virtual void draw( UNUSED class Drawable* d) {};

		virtual ~DrawableDecoration() {}
};

class Drawable {

	Drawable* _parent;   // Parent Drawable
	Drawable* _children; // list of child drawables

	Drawable* _next; // Next Drawable is the parents child list
	Drawable* _prev; // Previous Drawable is the parents child list
	unsigned int _events;

	void postEvent(unsigned int event);

protected:
	bool             _visible;
	bool             _transparent;
	bool             _modal;

	void invalidateOverlapped(void);
	void update(void);
	virtual void erase(void);
	uint32_t _next_update;

	// Decoration aware functions
	int absx_deco(void);
	int absy_deco(void);
	int width_deco(void);
	int height_deco(void);

	// Real handlers for events
	void real_invalidate(void);
	void real_show(void);
	void real_hide(void);
	void real_focus(void);
	void real_setBG(int col);
	void real_setFG(int col);

	virtual void focus_in(void);
	virtual void focus_out(void);

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
	bool isFocus(void);

	void addChild(Drawable& child);

	void removeChild(Drawable& child);

	void setXY(int x, int y);

	void setBG(int col);
	void setFG(int col);

	/**
	 * \brief Invalidate the drawable and scheduele it to be redrawn
	 */
	void invalidate(void);
	void show(void);
	void hide(void);

	void invalidate_elapsed(int ms);

	void processEvents(void);

	void focus(void);

	virtual bool key_event( UNUSED const AosEvent* event) { return false; }

	int width(void)  { return _width;  }
	int height(void) { return _height; }

	Drawable* next(void) { return _next; }
	Drawable* prev(void) { return _prev; }

	virtual ~Drawable(void);

	struct Box intersection(Drawable& d);

	friend class UGui;
};


class LineDecoration : public DrawableDecoration {
	unsigned int _color;
public:
	LineDecoration(int size, unsigned int color);
	void draw(Drawable* d);
};


class TextDecoration : public DrawableDecoration {
	char _text[64];
	const aostk_font* _font;
public:
	TextDecoration(const aostk_font* font,const char* text = NULL);
	void draw(Drawable* d);
	void setText(const char* new_text);
};

#endif

#endif
