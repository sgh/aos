#include <stdio.h>
#include <ugui/ugui_font.h>

//#define UTF8_DEBUG

int decode_utf8(const unsigned char** pptr) {
	unsigned char* ptr = *pptr;
	unsigned char c;
	int code_length = 0;
	unsigned int value = 0;

#ifdef UTF8_DEBUG
	printf("%s:\n",__FUNCTION__);
	printf("::0x%02X\n", *ptr);
#endif
	// Wind over UTF-parts
	while ((*ptr & 0xC0) == 0x80) {
		ptr++;

#ifdef UTF8_DEBUG
		printf("::0x%02X\n", *ptr);
#endif
	}

	// Count code length
	for (c = *ptr; c & 0x80; c<<=1)
		code_length++;

	// Ascii
	if (code_length==0) { 
		if (*ptr == 0)
			*pptr = ptr;
		else
			*pptr = ptr + 1;
		return *ptr;
	}

	value = *ptr << code_length;
	value &= 0xFF;
	value >>= code_length;

	while (--code_length) {
		ptr++;
#ifdef UTF8_DEBUG
		printf("::0x%02X\n", *ptr);
#endif
		if (*ptr == 0) {
			*pptr = ptr;
			return *ptr;
		}
		value <<= 6;
		value |= *ptr & 0x3F;
	}

#ifdef UTF8_DEBUG
	printf("Decoded unicode 0x%02X\n", value);
#endif
	*pptr = ptr+1;
	return value;
}


// int main() {
// 	unsigned char buf[6] = {0xC2, 0xA2, 0xE2, 0x82, 0xAC, 0x0};
// 	unsigned char* ptr = buf;
// 	int c;
// 	do {
// 		c = decode_utf8(&ptr);
// 		printf("0x%02X\n", c);
// 	} while (c);
// 	return 0;
// }
