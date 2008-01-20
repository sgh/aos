#ifndef AOSTK_CANVAS_H
#define AOSTK_CANVAS_H

#include <aostk_widget.h>

struct aostk_canvas {
	struct aostk_widget widget;
};

struct aostk_canvas* aostk_canvas_create(int width, int height, void* func);

#endif
