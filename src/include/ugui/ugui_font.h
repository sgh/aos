/*
	AOS - ARM Operating System
	Copyright (C) 2007  Søren Holm (sgh@sgh.dk)

	License: wxWindows Library Licence, Version 3.1
*/
#ifndef UGUI_UGUI_FONT_H
#define UGUI_UGUI_FONT_H

#include <stdint.h>
#include <aos/macros.h>
#include <string.h>
#include <utf8.h>

#ifdef __cplusplus
extern "C" {
#endif

struct aostk_point {
  unsigned int x;
  unsigned int y;
};

struct PACKED aostk_point_u8 {
  uint8_t x;
  uint8_t y;
};

struct PACKED aostk_size {
  unsigned int width;
  unsigned int height;
};

struct PACKED aostk_size_u8 {
  uint8_t width;
  uint8_t height;
};

struct PACKED aostk_glyph {
	uint16_t i;
	struct aostk_size_u8 size;
	int8_t top;
	struct aostk_point_u8 advance;
	uint8_t pitch;
	const unsigned char* data;
	int8_t left;
};


struct PACKED aostk_font {
	uint16_t numglyphs;
	uint8_t height;
	const struct aostk_glyph* glyphs;
};

static inline uint8_t ugui_font_height(const struct aostk_font* f) {
	return f->height;
}

static inline int ugui_strlen(const char* str) {
	return strlen(str);
}


void ugui_cursorat(const struct aostk_font* font, int x, int y, const char* buf, int cursorpos);

unsigned int ugui_font_strwidth(const struct aostk_font* f, const char* str);

uint8_t ugui_font_charwidth(const struct aostk_font* f, unsigned int c);

void ugui_putstring(const struct aostk_font* font, int x, int y, const char* str);

void ugui_putchar(const struct aostk_font* font, int x, int y, unsigned int ch);


struct unicode_parser {
	struct utf8_parser prev;
	struct utf8_parser current;
	struct utf8_parser next;
	uint32_t unicode;
};

void unicode_init(struct unicode_parser* parser, const char* str);
char unicode_next(struct unicode_parser* parser);
char unicode_prev(struct unicode_parser* parser);

static inline uint32_t unicode_current(const struct unicode_parser* parser) {
	return parser->unicode;
}


#ifdef __cplusplus
	}
#endif

#endif
