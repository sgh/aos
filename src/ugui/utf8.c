
int decode_utf8(unsigned char** ptr) {
	unsigned char = **ptr;
	int code_length = 1;
	unsigned int value = 0;
	while (code_length & 0x80) {
		code_length <<= 1;
		len++;
	}

	while (code_length--) {
		value <<= 8 - code_length;
	}
	

	return;
}


int main() {
}
