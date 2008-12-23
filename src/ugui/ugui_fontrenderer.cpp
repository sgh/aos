#include <stdlib.h>
//#include <types.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "ugui/ugui_font.h"
#include "ugui/ugui.h"

//void aostk_bitmap_raster(struct aostk_font* f, unsigned int posx, unsigned int posy, unsigned char c, int scanlines);

struct ppix8_native raster_ppix8;
static inline void aostk_raster(const struct aostk_glyph* glyph, int x, int y) {
	const char* r;
	unsigned int rows = glyph->size.height;
	int gwidth;
	const char* ptr;

	raster_ppix8.y = y - glyph->top;
	x += glyph->left;
	r = glyph->data;
	gwidth = glyph->size.width + x;

	while (rows--) {
		ptr = r;
		r += glyph->pitch;
		for (raster_ppix8.x=x; raster_ppix8.x<gwidth; raster_ppix8.x+=8) {
			raster_ppix8.bitmap = *ptr;
			ptr++;
			ugui_putpixel8_native(&raster_ppix8);
		}
		raster_ppix8.y++;
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
	unsigned int color;
	unsigned int outline;
  const struct aostk_glyph* g;
  assert(f != NULL);
  /**
   * Y-position is default not the baseline, but the topmost pixel of the font
   * So calculate the baseline by adding the font height
   */
  y += font->height;

	raster_ppix8.color = color = ugui_alloc_color(current_context->text_color);
	outline  = ugui_alloc_color(current_context->text_outline);
	bool draw_outline = (color != outline);
  while ((*str) != 0) {
    g = aostk_get_glyph(font, decode_utf8((const unsigned char**)&str));

		if (draw_outline) {
			raster_ppix8.color = outline;
  	  aostk_raster(g, x+1, y);
  	  aostk_raster(g, x-1, y);
  	  aostk_raster(g, x, y+1);
  	  aostk_raster(g, x, y-1);
			raster_ppix8.color = color;
		}

    aostk_raster(g, x, y);
    x += g->advance.x;
  }
}
