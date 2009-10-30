#include <stdio.h>
#include <ugui/ugui_font.h>

//#define UTF8_DEBUG

int decode_utf8(const unsigned char* ptr, unsigned int* unicode) {
	unsigned char c;
	int code_length = 0;
	unsigned int value = 0;
	int retval;

//#ifdef UTF8_DEBUG
//	printf("%s:\n",__FUNCTION__);
//	printf("::0x%02X\n", *ptr);
//#endif
	// Wind over UTF-parts
	while ((*ptr & 0xC0) == 0x80) {
		ptr++;

//#ifdef UTF8_DEBUG
//		printf("::0x%02X\n", *ptr);
//#endif
	}

	// Count code length
	for (c = *ptr; c & 0x80; c<<=1)
		code_length++;

	retval = code_length;
	
	// Ascii
	if (code_length==0) { 
// 		if (*ptr == 0)
// 			*pptr = ptr;
// 		else
// 			*pptr = ptr + 1;
		*unicode = *ptr;
		return 1;
	}

	value = *ptr << code_length;
	value &= 0xFF;
	value >>= code_length;

	while (--code_length) {
		ptr++;
//#ifdef UTF8_DEBUG
//		printf("::0x%02X\n", *ptr);
//#endif
		if (*ptr == 0) {
// 			*pptr = ptr;
			*unicode = *ptr;
			return code_length;
		}
		value <<= 6;
		value |= *ptr & 0x3F;
	}

// #ifdef UTF8_DEBUG
// 	printf("Decoded unicode 0x%02X\n", value);
// #endif

	*unicode = value;
	return retval;
}


#ifdef UTF8_DEBUG
int main() {
// 	unsigned char* buf = "Язык";
	unsigned char* buf = "HelloحWorld";
	const unsigned char* ptr = buf;
	unsigned int c;
	int len;
	do {
 		len = decode_utf8(ptr, &c);
 		printf("len %d 0x%02X\n",len, c);
		ptr+=len;
 	} while (c);
 	return 0;
}
#endif
