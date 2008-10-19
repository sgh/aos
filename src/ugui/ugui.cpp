#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include "ugui/ugui.h"

struct DrawingContext* current_context;

UGui* UGui::_instance;
mutex_t UGui::_eventlock;
mutex_t UGui::_drawlock;
semaphore_t UGui::_process_sem;

UGui::UGui() : _root(NULL), _focus_drawable(NULL) {
	mutex_init(&_eventlock);
	mutex_init(&_drawlock);
	sem_init(&_process_sem, 0);
}

void UGui::pushEvent(void) {
	sem_up(&_process_sem);
}

UGui* UGui::instance(void) {
	if (!_instance)
		_instance = new UGui();
	return _instance;
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


void UGui::processEvents(Drawable* d) {
	while (d) {
#warning if no screen activity is happening predraw functions will not be called FIX IT !!!!!!!!!!!!!!
#warning if no screen activity is happening predraw functions will not be called FIX IT !!!!!!!!!!!!!!
#warning if no screen activity is happening predraw functions will not be called FIX IT !!!!!!!!!!!!!!
#warning if no screen activity is happening predraw functions will not be called FIX IT !!!!!!!!!!!!!!
#warning if no screen activity is happening predraw functions will not be called FIX IT !!!!!!!!!!!!!!
		d->predraw();
		eventLock();
		d->processEvents();
		eventUnlock();
		processEvents(d->_children);
		d = d->next();
	}
}

void UGui::addRoot(Drawable& child) {
	drawLock();
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
	drawUnlock();
}

void UGui::removeRoot(Drawable& child) {
	drawLock();
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
	drawUnlock();
}

int UGui::eventLoop(void) {
	_draw_activity = 0;
	_update_min.x = INT32_MAX;
	_update_min.y = INT32_MAX;
	_update_max.x = INT32_MIN;
	_update_max.y = INT32_MIN;

	sem_timeout_down(&_process_sem, 100);

	processEvents(_root);

	drawLock();
	drawTraverse(_root);
	drawUnlock();
	
	return _draw_activity;
// 	printf("\n");
}

void UGui::eventLock(void) {
	mutex_lock(&_eventlock);
}

void UGui::eventUnlock(void) {
	mutex_unlock(&_eventlock);
}

void UGui::drawLock(void) {
	mutex_lock(&_drawlock);
}

void UGui::drawUnlock(void) {
	mutex_unlock(&_drawlock);
}



#warning add functionallity to invalidate everything.
