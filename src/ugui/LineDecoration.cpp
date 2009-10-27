#include <ugui/Drawable.h>
#include <ugui/ugui.h>

LineDecoration::LineDecoration(int size, unsigned int color) {
	_color = color;
	_top = size;
	_bottom = size;
	_left = size;
	_right = size;
}

#define maxX(d) ((d)->width() - 1 + _left + _right)
#define maxY(d) ((d)->height() - 1 + _top + _bottom)

void LineDecoration::draw(Drawable* d) {
	for (int i=0; i<_top; i++) {
		ugui_line(i, i, maxX(d)-i, i, _color); // Top
		ugui_line(i, maxY(d)-i, maxX(d)-i, maxY(d)-i, _color); // Bottom
		ugui_line(i, i, i, maxY(d)-i, _color ); // Left
		ugui_line(maxX(d) - i, i, maxX(d)-i, maxY(d)-i, _color ); // Right
	}
}
