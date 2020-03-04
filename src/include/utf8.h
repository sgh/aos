/*
	AOS - ARM Operating System
	Copyright (C) 2007  Søren Holm (sgh@sgh.dk)

	License: wxWindows Library Licence, Version 3.1
*/
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>


struct utf8_parser {
	const uint8_t* str;    /** \brief Pointer to the string */
	const uint8_t* ptr;    /** \brief Current location in the string */
	uint32_t unicode;      /** \brief Current unicode symbol */
};


void utf8_init(struct utf8_parser* parser, const char* str);
char utf8_next(struct utf8_parser* parser);
char utf8_prev(struct utf8_parser* parser);

static inline uint32_t utf8_current(const struct utf8_parser* parser) {
	return parser->unicode;
}

#ifdef __cplusplus
	}
#endif
