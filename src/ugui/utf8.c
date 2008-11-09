#include <stdio.h>
#include <ugui/ugui_font.h> 

int decode_utf8(const unsigned char** pptr) {
	unsigned char* ptr = *pptr;
	unsigned char c;
	int code_length = 0;
	unsigned int value = 0;

	printf("%s:",__FUNCTION__);
	// Wind over UTF-part
	while ((*ptr & 0xC0) == 0x80)
		ptr++;

	c = *ptr;
	while (c & 0x80) {
		c <<= 1;
		code_length++;
	}

	if (code_length > 1) {
		value = *ptr << code_length;
		value &= 0xFF;
		value >>= code_length;
		printf("UTF code length: %d\n", code_length);

		while (--code_length) {
			ptr++;
			if (*ptr == 0) {
				*pptr = ptr;
				return *ptr;
			}
			value <<= 6;
			value |= *ptr & 0x3F;
		}
	} else {
		value = *ptr;
// 		printf("Ascii: \n");
	}

	printf("0x%02X\n", value);
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
