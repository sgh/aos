#ifndef AOSTK_FONT_H
#define AOSTK_FONT_H

#include <stdint.h>
#include <aos/macros.h>
#include <string.h>

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


unsigned int ugui_font_strwidth(const struct aostk_font* f, const char* str);

uint8_t ugui_font_charwidth(const struct aostk_font* f, unsigned int c);

void ugui_putstring(const struct aostk_font* font, int x, int y, const char* str);

void ugui_putchar(const struct aostk_font* font, int x, int y, unsigned int ch);

// UTF-8 relative functions


struct utf8_parser {
	const uint8_t* str;  /** \brief Pointer to the string */
	const uint8_t* ptr;  /** \brief Current location in the string */
	uint32_t unicode;    /** \brief Unicode symbol */
};

char utf8_next(struct utf8_parser* parser);
char utf8_prev(struct utf8_parser* parser);

static inline uint32_t utf8_current(const struct utf8_parser* parser) {
	return parser->unicode;
}

#ifdef __cplusplus
	}
#endif

#endif
