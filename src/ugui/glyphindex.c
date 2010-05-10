#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ugui/ugui_font.h>

unsigned char active_characters[65536/8];

int main() {
  unsigned char buf[1024];
  int idx = 0;

	memset(active_characters, 0, sizeof(active_characters));
	while (!feof(stdin)) {
		buf[idx] = getchar();

		// Unicode characters must be collected in a single chunk
		if (buf[idx] & 0x80)
			idx++;
		else { // Normal ascii char or end of utf8
			// End of utf8
			if (idx > 0) {
				struct unicode_parser utf8;
				buf[idx+1] = 0;
				unicode_init(&utf8, (const char*)buf);
				do {
					unsigned uc = unicode_current(&utf8);
					active_characters[uc>>3] |= 1<<(uc&0x7);
// 					printf("%d ", uc);
				} while (unicode_next(&utf8));
				idx = 0;
			} else { // Normal character
				active_characters[buf[idx]>>3] |= 1<<(buf[idx]&0x7);
// 				printf("%c : %d\n", buf[idx], buf[idx]);
			}
		}
	}

	int i;
	int b;
	for (i=0; i<65536/8; i++) {
		for (b=0; b<8; b++) {
			if (active_characters[i] & 1<<b)
				printf("%d\n", i*8+b);
		}
	}
	return 0;
}

