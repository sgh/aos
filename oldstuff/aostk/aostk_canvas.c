#include <aostk_canvas.h>
#include <aostk_widget.h>
#include <stdlib.h>
#include <string.h>

static void aostk_canvas_init(struct aostk_canvas* c, int width, int height, void* draw) {
	aostk_widget_init(&c->widget, NULL, 0, 0, width, height);
	c->widget.size.width = width;
	c->widget.size.height = height;
	c->widget.draw = draw;
	c->widget.name = "CANVAS";
}

struct aostk_canvas* aostk_canvas_create(int width, int height, void* draw) {
	struct aostk_canvas* c = malloc_zero(sizeof(struct aostk_canvas));
	aostk_canvas_init(c, width, height, draw);
	return c;
}
