#ifndef AOSTK_FONTRENDERER_H
#define AOSTK_FONTRENDERER_H

#include <aostk.h>
#include <aostk_font.h>
#include <aostk_widget.h>

void aostk_putglyph(const struct aostk_glyph* g, unsigned int x, unsigned int y, unsigned int c, int scanlines, struct aostk_widget* w);

const struct aostk_glyph* aostk_get_glyph(struct aostk_font* f, unsigned int c);

#endif
