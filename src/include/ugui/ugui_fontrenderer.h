#ifndef AOSTK_FONTRENDERER_H
#define AOSTK_FONTRENDERER_H

#include "ugui_font.h"

#ifdef _cplusplus_
	extern "C" {
#endif

void aostk_putglyph(const struct aostk_glyph* g, unsigned int x, unsigned int y, unsigned int c, int scanlines);

const struct aostk_glyph* aostk_get_glyph(const struct aostk_font* f, unsigned int c);

#ifdef _cplusplus_
	}
#endif

#endif
