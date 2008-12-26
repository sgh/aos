
#ifndef UGUI_H
#define UGUI_H

#ifdef __cplusplus

#include "Drawable.h"
#include <aos/mutex.h>
#include <aos/semaphore.h>

extern struct DrawingContext* current_context;

// void ugui_lock(void);
// void ugui_unlock(void);

class UGui {
	int _draw_activity;
	Drawable* _root;
	static mutex_t _eventlock;
	static mutex_t _drawlock;
	static semaphore_t _process_sem;
	static UGui* _instance;

	UGui();

	void processEvents(void);

public:
	Drawable* _focus_drawable;
	struct Point _update_min;
	struct Point _update_max;
	static int    _event_time;
	static int    _draw_time;

	static UGui* instance(void);

	void processEvents(Drawable* d);

	void drawTraverse(Drawable* d);

  void key_event(const AosEvent* event);

	int eventLoop(void);

	void pushEvent(void);

	void addRoot(Drawable& child);

	void removeRoot(Drawable& child);
	
	void eventLock(void);
	void eventUnlock(void);
	void drawLock(void);
	void drawUnlock(void);
};
#endif

#ifdef __cplusplus
	extern "C" {
#endif

struct ppix8_native {
	unsigned int color;
	int x;
	int y;
	unsigned char bitmap;
};

void ugui_line(int x1, int y1, int x2, int y2, unsigned int color);
void ugui_putpixel(int x, int y, unsigned int color);
void ugui_fill(int x1, int y1, int width, int height, unsigned int color);
void ugui_set_bounds(int x1, int y1, int x2, int y2);
unsigned int ugui_alloc_color(unsigned int rgb);

// Native callbacks
void ugui_putpixel16_native(uint16_t bitmap, int x, int y, unsigned char color);

#ifdef __cplusplus
	}
#endif

#endif
