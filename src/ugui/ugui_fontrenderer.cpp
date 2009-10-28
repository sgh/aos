#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "ugui/ugui_font.h"
#include "ugui/ugui.h"

char right_to_left = 1;

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

static inline const struct aostk_glyph* aostk_get_glyph(const struct aostk_font* f, unsigned int c, unsigned int prev, unsigned int next) {
	unsigned int high = f->numglyphs - 1;
	unsigned int low = 0;
	unsigned int pivot;
	
	if (c == 0x641) {
		if (prev) {
			if (next)
				c = 0xFEF4; // Middle of a word
			else
				c = 0xFEF2; // End of a word
		} else {
			if (next)
				c = 0xFEF3; // Beginning of a word
			else
				c = 0xFEF1; // Isolated letter
		}
	}

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

uint8_t ugui_font_charwidth(const struct aostk_font* f, unsigned int c) {
	const struct aostk_glyph* g;
	g = aostk_get_glyph(f, c, 0, 0);
	return g->advance.x;
}

unsigned int ugui_font_strwidth(const struct aostk_font* font, const char* str) {
	unsigned int width = 0;
	const struct aostk_glyph* g;

	assert(str);
	assert(f);

	while (*str) {
    g = aostk_get_glyph(font, decode_utf8((const unsigned char**)&str), 0, 0);
		width += g->advance.x;
	}
	return width;
}

void ugui_putstring(const struct aostk_font* font, int x, int y, const char* str) {
	unsigned int color;
	unsigned int prev_char = 0;
	unsigned int next_char = 0;
	unsigned int outline_color;
  const struct aostk_glyph* g;
  assert(f != NULL);
  /**
   * Y-position is default not the baseline, but the topmost pixel of the font
   * So calculate the baseline by adding the font height
   */
  y += font->height;

	color = ugui_alloc_color(current_context->text_color);
	outline_color  = ugui_alloc_color(current_context->text_outline);
	bool draw_outline = (color != outline_color);
  while (*str) {

    g = aostk_get_glyph(font, decode_utf8((const unsigned char**)&str), prev_char, next_char);
// 		prev_char = ;
		
		// Right-to-left text must be pre-advanced
		if (right_to_left)
			x -= g->advance.x;

		if (draw_outline) {
  	  ugui_raster(g, x+1, y, outline_color);
  	  ugui_raster(g, x-1, y, outline_color);
  	  ugui_raster(g, x, y+1, outline_color);
  	  ugui_raster(g, x, y-1, outline_color);
		}

    ugui_raster(g, x, y, color);
		
		// Normal left-to-right text must by post-advanced
		if (!right_to_left)
			x += g->advance.x;
  }
}

void ugui_putchar(const struct aostk_font* font, int x, int y, unsigned int ch) {
	unsigned int color;
	unsigned int outline;
  const struct aostk_glyph* g;
  assert(f != NULL);
  /**
   * Y-position is default not the baseline, but the topmost pixel of the font
   * So calculate the baseline by adding the font height
   */
  y += font->height;

	color = ugui_alloc_color(current_context->text_color);
	outline  = ugui_alloc_color(current_context->text_outline);
	bool draw_outline = (color != outline);
  g = aostk_get_glyph(font, ch, 0, 0);
	if (draw_outline) {
 	  ugui_raster(g, x+1, y, outline);
 	  ugui_raster(g, x-1, y, outline);
 	  ugui_raster(g, x, y+1, outline);
 	  ugui_raster(g, x, y-1, outline);
	}

	ugui_raster(g, x, y, color);
}
