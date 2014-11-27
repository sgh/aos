/*
	AOS - ARM Operating System
	Copyright (C) 2007  Søren Holm (sgh@sgh.dk)

	License: wxWindows Library Licence, Version 3.1
*/
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <ugui/ugui_font.h>


static char connects_prev_next(unsigned int c, int direction) {
	// TODO
	return 0;
}


static unsigned int contextual_forms[] = {
// http://en.wikipedia.org/wiki/Arabic_Unicode#
//
// The arabic contextual forms are calculated as follows
// fx. for 0x628 the isolated form is 0xFE8F
// * Connection to previouse char 0xFE8F + 1
// * Connection to next char 0xFE8F + 2
// * Connection to both next and previous char 0xFE8F + 3

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


static unsigned int contextual_forms_nomiddle_noend[] = {
// Some characters have no beginning or middle contextual form.
	0x625,
	0x627,
	0x630,
	0x631,
	0x632,
	0x648,
	0x622,
	0x629,
	0x649
};

static unsigned int ligatures[] = {
	0x644, 0x625, 0xFEF9
};


void unicode_decode(struct unicode_parser* parser) {
	unsigned int prev    = utf8_current(&parser->prev);
	unsigned int current = utf8_current(&parser->current);
	unsigned int next    = utf8_current(&parser->next);
	unsigned char prev_connects = 0;   // Can the previous character connect to me
	unsigned char next_connects = 0;   // Can the previous character connect to me
	unsigned char i_connect_left = 1;  // Can I connect left
	unsigned char i_connect_right = 1; // Can I connect right
	int idx;
	int i;
	char found = 0;

	parser->unicode = current;
return;
	// TODO This only applies to arabic chars
	// previous and next chars only connect if they are not whitespace or start/end of line
	prev_connects = connects_prev_next(prev, -1);
	next_connects = connects_prev_next(next, 1);

	// Find Ligatures
	for (idx=0; idx<sizeof(ligatures) / sizeof(ligatures[0]); idx+=3) {
		if (ligatures[idx] == current && ligatures[idx+1] == next) {
			utf8_next(&parser->next);
			parser->unicode = ligatures[idx+2];
			return;
		}
	}

	for (idx=0; idx<sizeof(contextual_forms) / sizeof(contextual_forms[0]); idx+=2) {
		if (contextual_forms[idx] == current) {
			found = 1;
			break;
		}
	}

	if (found) {
		// Characters which do only connect left
		for (i=0; i<sizeof(contextual_forms_nomiddle_noend) / sizeof(contextual_forms_nomiddle_noend[0]); i++) {
				if (current == contextual_forms_nomiddle_noend[i]) {
					i_connect_right = 0;
				}
				if (prev == contextual_forms_nomiddle_noend[i]) {
					prev_connects = 0;
				}
		}

		// If only previous an I can connect
		if (prev_connects && i_connect_left && !(i_connect_right && next_connects))
			current = contextual_forms[idx+1] + 1;

		// If only next an I can connect
		if (!(prev_connects && i_connect_left) && i_connect_right && next_connects)
			current = contextual_forms[idx+1] + 2;

		// Both I and previous and next char can connect
		if (prev_connects && i_connect_left && i_connect_right && next_connects)
			current = contextual_forms[idx+1] + 3;
	}

	parser->unicode = current;
}

void unicode_init(struct unicode_parser* parser, const char* str) {
	memset(&parser->prev, 0, sizeof(parser->prev));
	utf8_init(&parser->current, str);
	parser->next = parser->current;
	utf8_next(&parser->next);
	parser->unicode = parser->current.unicode;
// 	unicode_decode(&parser);
}

char unicode_next(struct unicode_parser* parser) {
	if (utf8_current(&parser->current)) {

		// Try to move to next symbol
		parser->prev = parser->current;
		parser->current = parser->next;
		utf8_next(&parser->next);
		unicode_decode(parser);
		return 1;
	}
	return 0;
}

char unicode_prev(struct unicode_parser* parser) {
	if (parser->current.ptr == parser->current.str)
		return 0;
	
	// Try to move to previous symbol
	parser->next = parser->current;
	parser->current = parser->prev;
	utf8_prev(&parser->prev);
	unicode_decode(parser);
	return 1;
}


// #define UNICODE_DEBUG
#ifdef UNICODE_DEBUG
int main() {
	struct unicode_parser utf8;
	uint32_t uc;
// 	unsigned char* buf = "Язык";
// 	unsigned char* buf = "Helloæ\xF0\xA4\xAD\xA2℃æحWorld";
	unsigned char* buf = "الإعداد XC-2";

	unicode_init(&utf8, buf);

	do {
		uc = unicode_current(&utf8);
		printf("%5X",uc);
	} while (unicode_next(&utf8));
	printf("\n");

	do {
		uc = unicode_current(&utf8);
		printf("%5X",uc);
	} while (unicode_prev(&utf8));
	printf("\n");
	
 	return 0;
}
#endif