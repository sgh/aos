#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include "ugui/ugui.h"

struct DrawingContext* current_context;

UGui::UGui() : _root(NULL), _focus_drawable(NULL) {
}


void UGui::drawTraverse(Drawable* d) {
	while (d) {
		int tmp;
// 		printf(".");
		if (d->redraw()) {
			_draw_activity++;

			if (d->_abs_xy.x < _update_min.x)
				_update_min.x = d->_abs_xy.x;

			if (d->_abs_xy.y < _update_min.y)
				_update_min.y = d->_abs_xy.y;

			tmp = d->_abs_xy.x + d->_width - 1;
			if (tmp > _update_max.x)
				_update_max.x = tmp;

			tmp = d->_abs_xy.y + d->_height - 1;
			if (tmp > _update_max.y)
				_update_max.y = tmp;
		}

		drawTraverse(d->_children);
		d = d->next();
	}
}

void UGui::key_event(struct extended_char* xchar) {
	if (_focus_drawable)
		_focus_drawable->key_event(xchar);
}


void UGui::pollTraverse(Drawable* d) {
	while (d) {
		d->predraw();
		pollTraverse(d->_children);
		d = d->next();
	}
}

void UGui::addRoot(Drawable& child) {
	Drawable* d = _root;

	while (d) {
		assert(d != &child);
		if (!d->_next)
			break;

		if (d->_next->_modal && !child._modal)
			break;

		d = d->_next;
	}
	
	child._parent = NULL;

	if (d) { // Not the first child
		child._prev = d;
		child._next = d->_next;
		d->_next = &child;
	} else { // First child
		_root = &child;
		child._prev = NULL;
		child._next = NULL;
	}

	child.update();
	child.invalidate();
}

void UGui::removeRoot(Drawable& child) {
	child.invalidate();
	child.invalidateOverlapped();

	// Setup links
	if (child._next)
		child._next->_prev = child._prev;
	if (child._prev)
		child._prev->_next = child._next;

	child._next = NULL;
	child._prev = NULL;
	child._parent = NULL;

	if (_root == &child)
		_root = NULL;
}

int UGui::eventLoop(void) {
	_draw_activity = 0;
	_update_min.x = INT32_MAX;
	_update_min.y = INT32_MAX;
	_update_max.x = INT32_MIN;
	_update_max.y = INT32_MIN;

// 	ugui_lock();
	pollTraverse(_root);
	drawTraverse(_root);
// 	ugui_unlock();
	
	return _draw_activity;
// 	printf("\n");
}


#warning add functionallity to invalidate everything.
