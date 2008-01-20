#include <aostk_textbox.h>
#include <aostk_widget.h>
#include <stdlib.h>
#include <string.h>
#include <aostk_font.h>

static void update_dimensions(struct aostk_textbox* l) {
	
	l->widget.size.width = 200;//aostk_font_strwidth(l->widget.gc.font, (unsigned char*)l->text);
	l->widget.size.height = 80;//aostk_font_height(l->widget.gc.font);
	aostk_widget_update(l->widget.parent);
}

static void putnstring(struct aostk_widget* w, int line, const char* str, int n) {
	char buf[80];

	memcpy(buf, str, n);
	buf[n] = 0;
	aostk_putstring(w, 0, line * w->gc.font->height, buf, 0 );
}


static void aostk_textbox_draw(struct aostk_widget* w) {
	int idx = 0;
	int pixwidth = 0;
	int pixheight = w->gc.font->height;
	int line = 0;
	
	int token_width = 0;
// 	const char* token = NULL;
	int token_count = 0;
	const int boxwidth = aostk_widget_width(w);
// 	const int boxheight = aostk_widget_height(w);
	struct aostk_textbox* tb = (struct aostk_textbox*)w;
// 	const char* str = "The big brown fox jumps over the lazy dog.";
	const char* str = tb->text;
	const char* ptr = str;

	aostk_widget_clear(w);

	while (1) {
		pixwidth += aostk_font_charwidth(w->gc.font, *ptr);
// 		printf("%d(%c) ", pixwidth, *ptr);
// 		printf(" \"%c\" ", *ptr);
		idx++;
		
		if (*ptr == ' ' || *ptr == '\0') {
			
			if (pixwidth > boxwidth || *ptr == '\0') {

				// If we reach end of string and this is the first token - display it
				if (*ptr == '\0' && pixwidth <= boxwidth)
					token_count = idx;

				ptr = str;

				str += token_count;
				
				if (*(str-1) == ' ')
					token_count--;

				putnstring(w, line, ptr, token_count);
				
				ptr = str - 1;

				// Cut away prefixing space during linefeed
				if (*str == ' ')
					str++;

				token_count = 0;
				pixwidth = 0;
				line++;
				pixheight += w->gc.font->height;
// 				if (pixheight > boxheight)
// 					break;
				idx = 0;
			}
			
			token_width = pixwidth;
			token_count = idx;
			
// 			printf("T(%d) ", token_count);

		}
		
		if (*ptr == '\0')
			break;

		ptr++;
	}
}

void aostk_textbox_settext(struct aostk_textbox* w, const char* text) {
	struct aostk_textbox* l = (struct aostk_textbox*)w;
	
	if (strcmp(text, l->text) == 0)
		return;

	l->text = text;

	update_dimensions(l);
	aostk_widget_repaint(&w->widget);
}

void aostk_textbox_init(struct aostk_textbox* l, const char* text) {
	aostk_widget_init(&l->widget, NULL, 0, 0, 0, 0);
	l->widget.draw = &aostk_textbox_draw;
	l->text = text;
	l->widget.is_translucent = 1; // Visual appearence is part glyphs and part parent
	l->widget.name = "TEXTBOX";

	update_dimensions(l);
}

struct aostk_textbox* aostk_textbox_create(const char* text) {
	struct aostk_textbox* tb = malloc_zero(sizeof(struct aostk_textbox));
	aostk_textbox_init(tb, text);
	return tb;
}
