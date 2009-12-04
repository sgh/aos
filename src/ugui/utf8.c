
// #define UTF8_DEBUG

#ifdef UTF8_DEBUG
#include <stdio.h>
#endif

#include <stdint.h>
#include <string.h>
#include <ugui/ugui_font.h>



static void utf8_decode(struct utf8_parser* parser) {
	const uint8_t* ptr = parser->ptr;
	uint32_t unicode = 0;
	uint8_t code_length = 0;
	uint8_t c;
	
	// Ascii
	if ((*ptr & 0x80) == 0) {
		parser->unicode = *ptr;
		return;
	}

	// Count code length
	for (c = *ptr; c&0x80; c<<=1)
		code_length++;

	unicode = *ptr;
	unicode &= (0x80>>(code_length)) - 1;

	while (--code_length) {
		ptr++;
		unicode <<= 6;

		// Check of misplaced NULL-character
		if (*ptr == 0) {
			unicode = 0;
			break;
		}
			
		unicode |=  *ptr & 0x3F;
	}

	parser->unicode = unicode;
}

static char utf8_search_next(struct utf8_parser* parser) {
	const uint8_t* ptr = parser->ptr;

	if (*ptr == 0)
		return 0;

	ptr++;

	// Run over all chars on the form 10xxxxxx
	while ( (*ptr & 0xC0) == 0x80 ) ptr++;

	parser->ptr = ptr;
	return 1;
}


static char utf8_search_prev(struct utf8_parser* parser) {
	const uint8_t* ptr = parser->ptr;

	if (ptr == parser->str)
		return 0;

	ptr--;

	// Run over all chars on the form 10xxxxxx
	while ( ((*ptr & 0xC0) == 0x80) && (ptr != parser->str) ) ptr--;

	parser->ptr = ptr;
	return 1;
}


void utf8_init(struct utf8_parser* parser, const char* str) {
	memset(parser, 0, sizeof(struct utf8_parser));
	parser->str = str;
	parser->ptr = str;
	utf8_decode(parser);
}

char utf8_next(struct utf8_parser* parser) {
	if (utf8_search_next(parser)) {
		utf8_decode(parser);
		return 1;
	}

	return 0;
}

char utf8_prev(struct utf8_parser* parser) {
	if (utf8_search_prev(parser)) {
		utf8_decode(parser);
		return 1;
	}

	return 0;
}


#ifdef UTF8_DEBUG
int main() {
	struct utf8_parser utf8;
	uint32_t uc;
// 	unsigned char* buf = "Язык";
	unsigned char* buf = "Helloæ\xF0\xA4\xAD\xA2℃æحWorld";

	utf8_init(&utf8, buf);
	
	do {
		uc = utf8_current(&utf8);
		printf("UC: %X\n",uc);
	} while (utf8_next(&utf8));

	do {
		uc = utf8_current(&utf8);
		printf("UC: %X\n",uc);
	} while (utf8_prev(&utf8));

 	return 0;
}
#endif
