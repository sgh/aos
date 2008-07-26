#include <stdlib.h>
//#include <types.h>
#include <assert.h>
#include <string.h>

#include "ugui/ugui_fontrenderer.h"
#include "ugui/ugui.h"

void aostk_bitmap_raster(struct aostk_font* f, unsigned int posx, unsigned int posy, unsigned char c, int scanlines);

static void my_draw_bitmap(const struct aostk_glyph* glyph, int x, int y) {
	const char* r;
	uint_fast8_t rows = glyph->size.height;
	uint_fast8_t pixel;
	uint_fast8_t bit;
	const char* ptr;
	int line = 0;

	x += glyph->left;
	y -= glyph->top;
	
	r = glyph->data;
	while (rows--) {
		bit = 128;
		ptr = r;
		for (pixel=0; pixel<glyph->size.width; pixel++) {
			if (*ptr & bit)
				ugui_putpixel(x + pixel, y + line, current_context->text_color);
			#warning place at least one line in a temporary buffer before drawing pixel
			bit >>= 1;
			if (bit == 0) {
				bit = 128;
				ptr++;
			}
		}
		r += glyph->pitch;
		line++;
	}
}

const struct aostk_glyph* aostk_get_glyph(struct aostk_font* f, unsigned int c) {
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
	return NULL;
}


static void aostk_ttf_raster(const struct aostk_glyph* g, unsigned int posx, unsigned int posy) {
	struct aostk_point p;
// 	unsigned int x;
// 	unsigned int y;

	if (g == NULL)
		return;

// 	p.x = posx + g->left;
// 	p.y = posy - g->top;
	my_draw_bitmap(g, posx, posy);
}


unsigned int aostk_font_charwidth(struct aostk_font* f, char c) {
	const struct aostk_glyph* g;
		g = aostk_get_glyph(f, c);
		assert(g);
		return g->advance.x;
}

unsigned int aostk_font_strwidth(struct aostk_font* f, const char* str) {
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


 void aostk_putglyph(const struct aostk_glyph* g, unsigned int posx, unsigned int posy, unsigned int c) {
	assert(g != NULL);
	
// 	f->raster(f, posx, posy, c, scanlines);
	aostk_ttf_raster(g, posx, posy);
}

extern struct aostk_font VeraMoBd;

void aostk_putstring(int x, int y, const char* str) {
  struct aostk_font* f = &VeraMoBd;
  const struct aostk_glyph* g;
  assert(f != NULL);
  /**
   * Y-position is default not the baseline, but the topmost pixel of the font
   * So calculate the baseline by adding the font height
   */
  y += f->height;


  while ((*str) != 0) {
    g = aostk_get_glyph(f, (uint8_t)*str);
    aostk_putglyph(g, x, y, (uint8_t)*str);
    x += g->advance.x;
    str++;
  }
}
