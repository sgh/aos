#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>

#include "ugui/ugui_font.h"
#include "ugui/ugui.h"

static void ugui_raster(const struct aostk_glyph* glyph, int x, int y, unsigned char color) {
	const unsigned char* r;
	unsigned int rows = glyph->size.height;
	int gwidth;
	const unsigned char* ptr;
	int _y = y - glyph->top;
	x += glyph->left;
	r = glyph->data;
	gwidth = glyph->size.width + x;
	
	// Check bounds
	if (ugui_bounds.x2 < gwidth)
		gwidth = ugui_bounds.x2;

	// TODO:Add bounding on non 8 bit boundary by adding a mask on ugui_putpixel16_native

	while (rows--) {
		ptr = r;
		r += glyph->pitch;
		for (int _x=x; _x<gwidth; _x+=16) {
			uint16_t mask = 0xFFFF;
			uint16_t bitmap;

			// Mask off bits if there is no more room
			if ((gwidth - _x) < 16)
				mask <<= 16 - (gwidth - _x);

			bitmap = ( ((*ptr) << 8) | (*(ptr+1)) ) & mask;

			ptr += 2;
			ugui_putpixel16_native(bitmap, _x, _y, color);
		}
		_y++;
	}
}


static signed char char_direction(unsigned int c, signed char prev_char_direction) {
	// Spaces inheric the previous direction
	// TODO Add support for correct detection of previous and next strong character
	// Spaces and weak characters should be aligned like this 
	// PrevWeak NextWeak Direction
	// LRT      LTR      LTR
	// LRT      RTL      Inherit main direction
	// RTL      LTR      Inherit main direction
	// RTL      RTL      RTL
	if (c == 32)
		return 1;
	
	// Arabic is right-to-left
	// Standard arabic
	if (c >= 0x600 && c<=0x6FF)
		return -1;

	// Arabic presentation forms A
	if (c >= 0xFB50 && c<=0xFDFF)
		return -1;

	// Arabic presentation forms B
	if (c >= 0xFE70 && c<=0xFEFF)
		return -1;
	
	if (c == 0)
		return 0;
	
	// Default direction for all other chars is left-to-right
	return 1;
}

static inline const struct aostk_glyph* aostk_get_glyph(const struct aostk_font* f, unsigned int c) {
	unsigned int high = f->numglyphs - 1;
	unsigned int low = 0;
	unsigned int pivot;

	while (high >= low) {
		pivot = (high + low) >> 1;
		
		if (f->glyphs[pivot].i > c)
			high = pivot - 1;
		else
		if (f->glyphs[pivot].i < c)
			low = pivot + 1;
		else
			return &f->glyphs[pivot];
	}

	return &f->glyphs[0];
}

void ugui_cursorat(const struct aostk_font* font, int x, int y, const char* buf, int cursorpos) {
	static unsigned char showcursor = 0;
	struct unicode_parser utf8;
	uint32_t uc;
	int position = 0;

	unicode_init(&utf8, buf);
	if (cursorpos != 0) {
		do {
			uc = unicode_current(&utf8);
			x += aostk_get_glyph(font, uc)->advance.x;
			position++;
		} while (position<cursorpos && unicode_next(&utf8));
	}

	if (position == cursorpos) {
		uc = unicode_current(&utf8);
		ugui_fill(x-1, y+1, 3, ugui_font_height(font), 0xFF0000);
	}

}

uint8_t ugui_font_charwidth(const struct aostk_font* f, unsigned int c) {
	return aostk_get_glyph(f, c)->advance.x;
}

unsigned int ugui_font_strwidth(const struct aostk_font* font, const char* str) {
	unsigned int width = 0;
	struct unicode_parser unicode;
	uint32_t uc;

	assert(str);
	assert(font);

	unicode_init(&unicode, str);
	while (uc = unicode_current(&unicode)) {
		uc = unicode_current(&unicode);
		width += aostk_get_glyph(font, uc)->advance.x;
		unicode_next(&unicode);
	}

	return width;
}

struct ugui_fontrender_state {
	const struct aostk_font* font;
	int x;
	int y;
	int segment_width;
	unsigned int outline_color;
	unsigned int color;
};

static void ugui_render_glyphs(struct ugui_fontrender_state* state, const char* str, int count, signed char text_direction) {
	int x = state->x;
	int y = state->y;
	struct unicode_parser unicode;
	unsigned int current_char = 0;
	const struct aostk_glyph* g;
	assert(state->font != NULL);
	
	if (text_direction == -1)
		x += state->segment_width;
	
	bool draw_outline = (state->color != state->outline_color);
	
	unicode_init(&unicode, str);

	while (count>0) {
		g = aostk_get_glyph(state->font, unicode_current(&unicode));
		unicode_next(&unicode);
		
		if (text_direction == -1)
			x -= g->advance.x;

		if (x >= ugui_bounds.x1 && x <= ugui_bounds.x2) {
			if (draw_outline) {
				ugui_raster(g, x+1, y, state->outline_color);
				ugui_raster(g, x-1, y, state->outline_color);
				ugui_raster(g, x, y+1, state->outline_color);
				ugui_raster(g, x, y-1, state->outline_color);
			}
			ugui_raster(g, x, y, state->color);
		}

		if (text_direction == 1)
			x += g->advance.x;

		count--;
	}
	
	if (text_direction == -1)
		x += state->segment_width;

	state->x = x;
	state->y = y;
}


void ugui_putstring(const struct aostk_font* font, int x, int y, const char* str) {
	unsigned int current_char = 0;
	signed char direction = 1;
	signed char tmp;
	char line_feed = 0;
	int char_count = 0;
	struct unicode_parser unicode;
	struct ugui_fontrender_state state;
	const uint8_t* current_ptr;
// 	int num =0;

	/**
	 * Y-position is default not the baseline, but the topmost pixel of the font
	 * So calculate the baseline by adding the font height
	 */
	y += font->height;
	
	state.font           = font;
	state.x              = x;
	state.y              = y;
	state.segment_width  = 0;
	state.color          = ugui_alloc_color(current_context->text_color);
	state.outline_color  = ugui_alloc_color(current_context->text_outline);;

	unicode_init(&unicode, str);
	direction = char_direction(unicode_current(&unicode), direction);
	do {
		current_char = unicode_current(&unicode);

		// Decode next unicode symbol
		unicode_next(&unicode);


		int tmp = direction;
		direction = char_direction(unicode_current(&unicode), direction);

		char_count++;
		// Now get the current glyphs width
		state.segment_width += aostk_get_glyph(state.font, current_char)->advance.x;

		if (current_char == '\n')
			line_feed = 1;
		
		if ((tmp != direction) || line_feed) {
			if (line_feed)
				char_count--;
			ugui_render_glyphs(&state, str, char_count, tmp);
			str = (const char*)unicode.current.ptr;
			char_count = 0;
			if (line_feed) {
				state.y += font->height;
				state.x = x;
				line_feed = 0;
			}
		}
		
	} while (current_char);

	

}


void ugui_putchar(const struct aostk_font* font, int x, int y, unsigned int ch) {
	unsigned int color;
	unsigned int outline;
  const struct aostk_glyph* g;

  /**
   * Y-position is default not the baseline, but the topmost pixel of the font
   * So calculate the baseline by adding the font height
   */
  y += font->height;

	color = ugui_alloc_color(current_context->text_color);
	outline  = ugui_alloc_color(current_context->text_outline);
	bool draw_outline = (color != outline);
  g = aostk_get_glyph(font, ch);
	if (draw_outline) {
 	  ugui_raster(g, x+1, y, outline);
 	  ugui_raster(g, x-1, y, outline);
 	  ugui_raster(g, x, y+1, outline);
 	  ugui_raster(g, x, y-1, outline);
	}

	ugui_raster(g, x, y, color);
}
