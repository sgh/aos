#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "ugui/ugui_font.h"
#include "ugui/ugui.h"

static signed char text_direction = -1;

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

static unsigned int arabic_context_forms[] = {
// http://en.wikipedia.org/wiki/Arabic_Unicode#
//
// The arabic contextual forms are calculated as follows
// fx. for 0x628 the isolated form is 0xFE8F
// * Connection to previouse char 0xFE8F + 1
// * Connection to next char 0xFE8F + 2
// * Connection to both next and previous char 0xFE8F + 3
//
	0x627, 0xFE8D, // Only conection to previous char
	0x628, 0xFE8F,
	0x62A, 0xFE95,
	0x62B, 0xFE99,
	0x62C, 0xFE9D,
	0x62D, 0xFEA1,
	0x62E, 0xFEA5,
	0x62F, 0xFEA9, // Only conection to previous char
	0x630, 0xFEAB, // Only conection to previous char
	0x631, 0xFEAD, // Only conection to previous char
	0x632, 0xFEAF, // Only conection to previous char
	0x633, 0xFEB1,
	0x634, 0xFEB5,
	0x635, 0xFEB9,
	0x636, 0xFEBD,
	0x637, 0xFEC1,
	0x638, 0xFEC5,
	0x639, 0xFEC9,
	0x63A, 0xFECD,
	0x641, 0xFED1,
	0x642, 0xFED5,
	0x643, 0xFED9,
	0x644, 0xFEDD,
	0x645, 0xFEE1,
	0x646, 0xFEE5,
	0x647, 0xFEE9,
	0x648, 0xFEED, // Only conection to previous char
	0x64A, 0xFEF1,
	0x622, 0xFE81,
	0x629, 0xFE93, // Only conection to previous char
	0x649, 0xFEEF, // Only conection to previous char
};


static unsigned int arabic_context_forms_nomiddle_noend[] = {
// Some characters have no beginning or middle contextual form.
	0x627,
	0x625,
	0x630,
	0x631,
	0x632,
	0x648,
	0x622,
	0x629,
	0x649
};


static char is_not_empty(unsigned int c) {
	if (c==0 || c==32)
		return 0;
	return 1;
}

static inline unsigned int contextual_forms(unsigned int c, unsigned int prev, unsigned int next) {

	// Arabic contextual form
	if (c >= 0x600 && c<=0x6FF) {
			unsigned char prev_connects = 0;   // Can the previous character connect to me
			unsigned char next_connects = 0;   // Can the previous character connect to me
			unsigned char i_connect_left = 1;  // Can I connect left
			unsigned char i_connect_right = 1; // Can I connect right
			int idx;
			
			// previous and next chars only connect if they are not whitespace or start/end of line
			if (is_not_empty(prev)) prev_connects = 1;
			if (is_not_empty(next)) next_connects = 1;

			for (idx=0; idx<sizeof(arabic_context_forms) / sizeof(arabic_context_forms[0]); idx+=2) {
				if (arabic_context_forms[idx] == c)
					break;
			}

			// Characters which do only connect left
			for (int i=0; i<sizeof(arabic_context_forms_nomiddle_noend) / sizeof(arabic_context_forms_nomiddle_noend[0]); i++) {
					if (c == arabic_context_forms_nomiddle_noend[i]) {
						i_connect_right = 0;
					}
					if (prev == arabic_context_forms_nomiddle_noend[i]) {
						prev_connects = 0;
					}
			}

			// If only previous an I can connect
			if (prev_connects && i_connect_left && !(i_connect_right && next_connects))
				c = arabic_context_forms[idx+1] + 1; 
			
			// If only next an I can connect
			if (!(prev_connects && i_connect_left) && i_connect_right && next_connects)
				c = arabic_context_forms[idx+1] + 2;
			
			// Both I and previous and next char can connect
			if (prev_connects && i_connect_left && i_connect_right && next_connects)
				c = arabic_context_forms[idx+1] + 3;

	}
	
	return c;
}


static inline const struct aostk_glyph* aostk_get_glyph(const struct aostk_font* f, unsigned int c, unsigned int prev, unsigned int next) {
	unsigned int high = f->numglyphs - 1;
	unsigned int low = 0;
	unsigned int pivot;
	
	c = contextual_forms(c, prev, next);

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
	unsigned int current_char = 0;
	unsigned int outline_color;
  const struct aostk_glyph* g;
  assert(f != NULL);
  /**
   * Y-position is default not the baseline, but the topmost pixel of the font
   * So calculate the baseline by adding the font height
   */
  y += font->height;
	
	// Start from the other side of the box if the text direction is right-to-left
	if (text_direction == -1)
		x = ugui_bounds.x2 - x;

	color = ugui_alloc_color(current_context->text_color);
	outline_color  = ugui_alloc_color(current_context->text_outline);
	
	bool draw_outline = (color != outline_color);
	
	current_char = decode_utf8((const unsigned char**)&str);
	
  while (current_char) {
		next_char = decode_utf8((const unsigned char**)&str);

    g = aostk_get_glyph(font, current_char, prev_char, next_char);
		
 		prev_char = current_char;
		current_char = next_char;
		
		// Right-to-left text must be pre-advanced
		if (text_direction == -1)
			x -= g->advance.x;

		if (draw_outline) {
  	  ugui_raster(g, x+1, y, outline_color);
  	  ugui_raster(g, x-1, y, outline_color);
  	  ugui_raster(g, x, y+1, outline_color);
  	  ugui_raster(g, x, y-1, outline_color);
		}

    ugui_raster(g, x, y, color);
		
		// Normal left-to-right text must by post-advanced
		if (text_direction == 1)
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
