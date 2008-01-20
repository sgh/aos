#ifndef AOSTK_FONT_H
#define AOSTK_FONT_H

#include <stdint.h>
#include <aostk.h>


struct aostk_glyph {
	uint16_t i;
	struct aostk_size size;
	int8_t top;
	uint8_t left;
	struct aostk_point advance;
	uint8_t pitch;
 const char* data;
};


struct aostk_font {
	uint16_t numglyphs;
	uint8_t height;
	const struct aostk_glyph* glyphs;
};

extern struct aostk_font verafont;
extern struct aostk_font verafontlarge;

static inline uint8_t aostk_font_height(struct aostk_font* f) {
	return f->height;
}

unsigned int aostk_font_strwidth(struct aostk_font* f, const char* str);

unsigned int aostk_font_charwidth(struct aostk_font* f, char c);

#endif
