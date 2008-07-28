
#ifndef UGUI_H
#define UGUI_H

#ifdef __cplusplus

#include "Drawable.h"

extern struct DrawingContext* current_context;

void ugui_lock(void);
void ugui_unlock(void);

class UGui {
	int _draw_activity;
	Drawable* _root;

public:
	struct Point _update_min;
	struct Point _update_max;

	void pollTraverse(Drawable* d);

	void drawTraverse(Drawable* d);

	int drawTraverseAll(void);

	void addRoot(Drawable& child);

	void removeRoot(Drawable& child);
	
	UGui();

};
#endif

#ifdef __cplusplus
	extern "C" {
#endif

void ugui_line(int x1, int y1, int x2, int y2, unsigned int color);
void ugui_putpixel(int x, int y, unsigned int color);
void ugui_fill(int x1, int y1, int width, int height, unsigned int color);
void ugui_set_bounds(int x1, int y1, int x2, int y2);
unsigned int ugui_alloc_color(unsigned int rgb);

#ifdef __cplusplus
	}
#endif

#endif
