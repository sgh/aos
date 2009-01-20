#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "ugui/ugui.h"
#include "ugui/Drawable.h"
#include "aos/aos.h"

#define INVALIDATE_EVENT 1
#define SHOW_EVENT       2
#define HIDE_EVENT       4
#define FOCUS_EVENT      8

Drawable::Drawable(int x, int y, int width, int height)
	: _parent(NULL), _children(NULL), _next(NULL), _prev(NULL), _events(0), _next_update(0xFFFFFFFF), _width(width), _height(height), _visible(true), _transparent(false), _modal(false), _dirty(0)  {
	memset(&_ctx, 0, sizeof(_ctx));
	_abs_xy.x = _rel_xy.x = x;
	_abs_xy.y = _rel_xy.y = y;
// 	setXY(x, y);
	_decoration = NULL;//(class DrawableDecoration*)&testdecoration;
}

bool Drawable::redraw(void) {
	if (_next_update != 0xFFFFFFFF) {
		uint32_t now;
		get_sysmtime(&now);
		if (_next_update <= now) {
			invalidate();
			_next_update = 0xFFFFFFFF;
		}
	}

	if (!_dirty)
		return false;

	int x1;
	int y1;
	int x2;
	int y2;

	// Setup for drawing decoration
	if (_decoration) {
// 		x1 = _abs_xy.x - _decoration->_left;
// 		y1 = _abs_xy.y - _decoration->_top;
// 		x2 = _abs_xy.x + _width  - 1 + _decoration->_right;
// 		y2 = _abs_xy.y + _height - 1 + _decoration->_bottom;
		x1 = absx_deco();
		y1 = absy_deco();
		x2 = x1 + width_deco() - 1;
		y2 = y1 + height_deco() -1;
		current_context = &_decoration->_ctx;
		#warning we must obey parent boundaries too
		ugui_set_bounds(x1, y1, x2, y2);
		_decoration->draw(this);
	}

	// Setup for drawing drawable content
	x1 = _abs_xy.x;
	y1 = _abs_xy.y;
	x2 = _abs_xy.x + _width  - 1;
	y2 = _abs_xy.y + _height - 1;
	current_context = &_ctx;
	#warning we must obey parent boundaries too
	ugui_set_bounds(x1, y1, x2, y2);

	//printf("Drawme r(%d,%d) a(%d,%d)\n", _rel_xy.x, _rel_xy.y, _abs_xy.x, _abs_xy.y);

	_dirty = false;
	if (!_transparent)
		erase();
	draw();

	current_context = NULL;

	return true;
}


void Drawable::addChild(Drawable& child) {
	UGui::instance()->drawLock();
	UGui::instance()->eventLock();
	Drawable* d = _children;

	while (d) {
		assert(d != &child);
		if (!d->_next)
			break;

		if (d->_next->_modal && !_modal)
   		break;

		d = d->_next;
	}
	
	child._parent = this;

	if (d) { // Not the first child
		d->_next = &child;
		child._prev = d;
		child._next = NULL;
	} else { // First child
		_children = &child;
		child._prev = NULL;
		child._next = NULL;
	}

	child.update();
	child.real_invalidate();
	UGui::instance()->eventUnlock();
	UGui::instance()->drawUnlock();
}

void Drawable::removeChild(Drawable& child) {
	UGui::instance()->drawLock();
	UGui::instance()->eventLock();
	real_invalidate();
	invalidateOverlapped();

	// If we are the first child point parent to next child
	if (child._parent && child._parent->_children == &child)
		child._parent->_children = child._next;

	// Setup links
	if (child._next)
		child._next->_prev = child._prev;
	if (child._prev)
		child._prev->_next = child._next;

// 	if (child._parent)
// 		child._parent->invalidate();

	child._next = NULL;
	child._prev = NULL;
	child._parent = NULL;
	UGui::instance()->eventUnlock();
	UGui::instance()->drawUnlock();
}

void Drawable::setXY(int x, int y) {
	invalidateOverlapped();
	_abs_xy.x = _rel_xy.x = x;
	_abs_xy.y = _rel_xy.y = y;
	update();
	real_invalidate();
}

void Drawable::update(void) {
	Drawable* d = _next;

	if (_parent) {
		_abs_xy.x = _parent->_abs_xy.x + _rel_xy.x;
		_abs_xy.y = _parent->_abs_xy.y + _rel_xy.y;
	}

	while (d) {
		d->update();
		d = d->_next;
	}
}

void Drawable::real_setBG(int col) {
	_ctx.bg_color = col;
}

void Drawable::setBG(int col) {
	real_setBG(col);
	invalidate();
}

void Drawable::real_setFG(int col) {
	_ctx.fg_color = col;
}

void Drawable::setFG(int col) {
	real_setFG(col);
	invalidate();
}

void Drawable::erase(void) {
	ugui_fill(0, 0, _width, _height, _ctx.bg_color);
}

void Drawable::real_show(void) {
	if (!isVisible()) {
		_visible = true;
		real_invalidate();
	}
}

void Drawable::real_hide(void) {
	if (isVisible()) {
		invalidateOverlapped();
		_visible = false;
		_dirty = false;
	}
}

void Drawable::real_focus() {
	UGui::instance()->_focus_drawable = this;
}

bool Drawable::isFocus() {
	return (UGui::instance()->_focus_drawable == this);
}

void Drawable::postEvent(unsigned int event) {
	UGui* g = UGui::instance();
	bool changed = 1;
	g->eventLock();

	// if already "shown" and a rapid hide-show sequence occurs, before the first hide event has been processed,
	// then remove the first event to make sure we end up with "shown".
	if ( (event & SHOW_EVENT) && (_events & HIDE_EVENT) )
		_events &= ~HIDE_EVENT;
	else // if already "hidden", handle rapid show-hide event correctly 
		if ( (event & HIDE_EVENT) && (_events & SHOW_EVENT) )
			_events  &= ~SHOW_EVENT;

	// if event is HIDE_EVENT and we already have a HIDE_EVENT pending in queue or we are already hidden, do nothing
	// and vice versa for SHOW_EVENT
	if ( ( (event & HIDE_EVENT) && ( (_events & HIDE_EVENT) || !isVisible() ) ) ||
		   ( (event & SHOW_EVENT) && ( (_events & SHOW_EVENT) ||  isVisible() ) ) )
	  changed = 0;

	if ( changed )
	{
		_events |= event;
		g->pushEvent();
	}

	g->eventUnlock();
}

void Drawable::focus(void) {
	UGui* g = UGui::instance();

	if (g->_focus_drawable)
		g->_focus_drawable->focus_out();

	focus_in();
	postEvent(FOCUS_EVENT);
}

void Drawable::focus_in(void) {
}

void Drawable::focus_out(void) {
}

void Drawable::invalidate(void) {
	postEvent(INVALIDATE_EVENT);
}

void Drawable::show(void) {
		postEvent(SHOW_EVENT);
}

void Drawable::hide(void) {
		postEvent(HIDE_EVENT);
}

void Drawable::processEvents(void) {
	if (_events & INVALIDATE_EVENT) real_invalidate();
	if (_events & SHOW_EVENT)       real_show();
	if (_events & HIDE_EVENT)       real_hide();
	if (_events & FOCUS_EVENT)      real_focus();
	_events = 0;
}

void Drawable::invalidate_elapsed(int ms) {
#warning FIX ME. It does not work with semaphore triggered eventloop
	uint32_t now;
	if (_dirty) return;
	get_sysmtime(&now);
	if (_next_update == 0xFFFFFFFF)
		_next_update = now + ms;
}

void Drawable::real_invalidate(void) {
	Drawable* d;

	// If already dirty just return
	// Non-visible drawable must not be dirty, since it will result in a redraw
	if (_dirty || !isVisible()) return;

	_dirty = true;

	if (_transparent)
		invalidateOverlapped();

	// Invalidate possibly overlapping preceding sisters and brothers
	d = _next;
	while (d) {
		struct Box box = intersection(*d);
		if ( (box.width > 0 && box.height > 0))
				d->real_invalidate();
		d = d->_next;
	}

	// Now invalidate children
	d = _children;
	while (d) {
		d->real_invalidate();
		d = d->_next;
	}

// Invalidate possibly overlapping toplevel widgets. See invalidateOverlapped
// for further description.
	d = this;
	while (d->_parent)
		d = d->_parent;

	d = d->_next;
	while (d) {
		struct Box box = intersection(*d);

		if ( (box.width > 0 && box.height > 0))
			d->real_invalidate();
		d = d->_next;
	}
}

bool Drawable::isVisible(void) {
	bool retval;
	retval =  _visible;
	return retval;
}

void Drawable::invalidateOverlapped(void) {
	// Invalidate possibly overlapped parents. Only widget draw before
	// this (_prev) will be considered.

	// Basically we only check to toplevel widgets, it could be improved
	// but since to do not buffer the image-data for all widgets a redraw of a
	// widget as effect of another widget disappearing needs to redraw everything
	// that widget was overlapping
	Drawable* d = this;

	while (d->_parent)
		d = d->_parent;

	while (d) {
		struct Box box = intersection(*d);

		if ( (box.width > 0 && box.height > 0))
				d->real_invalidate();
		d = d->_prev;
	}

}

int Drawable::absx_deco(void) {
	int x = _abs_xy.x;

	if (_decoration)
		x -= _decoration->_left;
	return x;
}

int Drawable::absy_deco(void) {
	int y = _abs_xy.y;

	if (_decoration)
		y -= _decoration->_top;
	return y;
}

int Drawable::width_deco(void) {
	int width = _width;

	if (_decoration) {
		width += _decoration->_right;
		width += _decoration->_left;
	}
	return width;
}

int Drawable::height_deco(void) {
	int height = _height;

	if (_decoration) {
		height += _decoration->_top;
		height += _decoration->_bottom;
	}
	return height;
}

struct Box Drawable::intersection(Drawable& d) {
	struct Box b;
	int wh1, wh2;

	if (!isVisible() || !d.isVisible()) {
		b.x = b.y = b.width = b.height = 0;
		return b;
	}

	// Set x and y to the highest value
	wh1 = d.absx_deco();
	wh2 = absx_deco();

	if (wh1 > wh2)
		b.x = wh1;
	else
		b.x = wh2;
	wh1 = d.absy_deco();
	wh2 = absy_deco();

	if (wh1 > wh2)
		b.y = wh1;
	else
		b.y = wh2;

	// Set width and height to the smallest xy-compensated value
	wh1 =  d.absx_deco() + d.width_deco();
	wh2 =  absx_deco() + width_deco();

	if ( wh1 < wh2)
		b.width = wh1;
	else
		b.width = wh2;

	b.width -= b.x;

	wh1 =  d.absy_deco() + d.height_deco();
	wh2 =  absy_deco() + height_deco();
	if ( wh1 < wh2)
		b.height = wh1;
	else
		b.height = wh2;

	b.height -= b.y;
	
	return b;
}
