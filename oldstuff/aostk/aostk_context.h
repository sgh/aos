#ifndef AOSTK_CONTEXT_H
#define AOSTK_CONTEXT_H

struct aostk_font;

struct aostk_gc {
	struct aostk_font* font;
	uint32_t bg_col;
	uint32_t fg_col;
};

static inline void aostk_gc_set_fg(struct aostk_gc* gc, uint32_t color) {
	gc->fg_col = color;
}

static inline void aostk_gc_set_bg(struct aostk_gc* gc, uint32_t color) {
	gc->bg_col = color;
}


#endif
