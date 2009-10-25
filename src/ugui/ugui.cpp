#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include "ugui/ugui.h"
#include <aos/aos.h>

struct UGuiBound ugui_bounds;
struct DrawingContext* current_context;

UGui*       UGui::_instance;
mutex_t     UGui::_eventlock;
mutex_t     UGui::_drawlock;
mutex_t     UGui::_process_lock;
int         UGui::_event_time;
int         UGui::_draw_time;

UGui::UGui() : _root(NULL), _focus_drawable(NULL) {
// 	fprintf(stdout,"UGui construction ... ");
// 	fflush(stdout);
	mutex_init(&_process_lock);
	mutex_init(&_eventlock);
	mutex_init(&_drawlock);
// 	fprintf(stdout,"ok\n");
}

void UGui::pushEvent(void) {
	mutex_unlock(&_process_lock);
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

			tmp = d->absx_deco();
			if (tmp < _update_min.x)
				_update_min.x = tmp;

			tmp = d->absy_deco();
			if (tmp < _update_min.y)
				_update_min.y = tmp;

    	tmp = d->absx_deco() + d->width_deco();
			if (tmp > _update_max.x)
				_update_max.x = tmp;

    	tmp = d->absy_deco() + d->height_deco();
			if (tmp > _update_max.y)
				_update_max.y = tmp;
		}

		if (d->isVisible())
			drawTraverse(d->_children);
		d = d->next();
	}
}

void UGui::key_event(const AosEvent* event) {
	if (_focus_drawable)
		_focus_drawable->key_event(event);
}


void UGui::processEvents(Drawable* d) {
	while (d) {
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
	eventLock();
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
	child.real_invalidate();
	eventUnlock();
	drawUnlock();
}

void UGui::removeRoot(Drawable& child) {
	drawLock();
	eventLock();
	child.real_invalidate();
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
	eventUnlock();
	drawUnlock();
}

int UGui::eventLoop(void) {
	uint32_t startTime;
	uint32_t endTime;
	_draw_activity = 0;
	_update_min.x = INT32_MAX;
	_update_min.y = INT32_MAX;
	_update_max.x = INT32_MIN;
	_update_max.y = INT32_MIN;

	//mutex_timeout_lock(&_process_lock, 100);
	
	get_sysmtime(&startTime);
	processEvents(_root);
	get_sysmtime(&endTime);

	_event_time = endTime - startTime;

	drawLock();
	get_sysmtime(&startTime);
	drawTraverse(_root);
	get_sysmtime(&endTime);
	drawUnlock();

	_draw_time = endTime - startTime;

	return _draw_activity;
// 	printf("\n");
}

void UGui::eventLock(void) {
// 	fprintf(stdout,"Locking eventLock ...");
// 	fflush(stdout);
	mutex_lock(&_eventlock);
// 	fprintf(stdout,"ok\n");
}

void UGui::eventUnlock(void) {
	mutex_unlock(&_eventlock);
}

void UGui::drawLock(void) {
// 	fprintf(stdout,"Locking eventLock ...");
// 	fflush(stdout);
	mutex_lock(&_drawlock);
// 	fprintf(stdout,"ok\n");
}

void UGui::drawUnlock(void) {
	mutex_unlock(&_drawlock);
}



#warning add functionallity to invalidate everything.
