#include <ugui/Drawable.h>
#include <ugui/ugui.h>
#include <ugui/ugui_font.h>

TextDecoration::TextDecoration(const aostk_font* font, const char* text) {
	_text[0] = 0;
	_top = 16;
	_bottom = 2;
	_left = 2;
	_right = 2;
	_font = font;
	if (text)
		setText(text);
}

#define maxX(d) ((d)->width() - 1 + _left + _right)
#define maxY(d) ((d)->height() - 1 + _top + _bottom)

void TextDecoration::draw(Drawable* d) {
	int width = maxX(d)+1;

	ugui_fill(0, 1, width, _top-1, 0x000000);
	ugui_line(1,0 , width - 2, 0, 0x000000);

	ugui_putstring(_font, 4, 1, _text);

	for (int i=0; i<2; i++) {
		unsigned int color = 0x000000;
		ugui_line(0, maxY(d) - i, maxX(d), maxY(d) - i, color);
		ugui_line(i, _top, i, maxY(d), color );
		ugui_line(maxX(d) - i, _top, maxX(d) - i, maxY(d), color);
	}
}

void TextDecoration::setText(const char* new_text) {
	_ctx.text_color = _ctx.text_outline = 0xFFFFFF;
	strcpy(_text, new_text);
}
