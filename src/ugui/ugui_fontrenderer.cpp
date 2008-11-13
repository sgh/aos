#include <stdlib.h>
//#include <types.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "ugui/ugui_font.h"
#include "ugui/ugui.h"

void aostk_bitmap_raster(struct aostk_font* f, unsigned int posx, unsigned int posy, unsigned char c, int scanlines);

static inline void aostk_raster(const struct aostk_glyph* glyph, unsigned int x, unsigned int y) {
	const char* r;
	unsigned int rows = glyph->size.height;
	int pixel;
	const char* ptr;
	int line = 0;
	register unsigned char data = *ptr;

	x += glyph->left;
	y -= glyph->top;
	
	r = glyph->data;
	while (rows--) {
		ptr = r;
		data = *ptr;
		pixel = 0;
		while (pixel<glyph->size.width) {
			ugui_putpixel8(x + pixel, y + line, current_context->text_color, data);
			ptr++;
			pixel += 8;
			data = *ptr;
		}
		r += glyph->pitch;
		line++;
	}
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

unsigned int aostk_font_charwidth(const struct aostk_font* f, char c) {
	const struct aostk_glyph* g;
		g = aostk_get_glyph(f, c);
		assert(g);
		return g->advance.x;
}

unsigned int aostk_font_strwidth(const struct aostk_font* f, const char* str) {
	unsigned int width = 0;
	const struct aostk_glyph* g;

	assert(str);
	assert(f);

	while (*str) {
		g = aostk_get_glyph(f, *str);
		assert(g);
		width += g->advance.x;
		str++;
	}
	return width;
}

void aostk_putstring(const struct aostk_font* font, int x, int y, const char* str) {
  const struct aostk_glyph* g;
  assert(f != NULL);
  /**
   * Y-position is default not the baseline, but the topmost pixel of the font
   * So calculate the baseline by adding the font height
   */
  y += font->height;


  while ((*str) != 0) {
    g = aostk_get_glyph(font, decode_utf8((const unsigned char**)&str));
    aostk_raster(g, x, y);
    x += g->advance.x;
  }
}
