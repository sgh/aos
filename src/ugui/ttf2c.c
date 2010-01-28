#include <ft2build.h>
#include FT_FREETYPE_H

#include <stdio.h>
#include <stdint.h>

#include "ugui/ugui_font.h"

#define TTF2C

#define MAX_GLYPHS 1024*30

struct aostk_glyph genglyphs[MAX_GLYPHS];

struct aostk_font genfont = {
	.numglyphs = 0,
 	.glyphs = genglyphs,
};


void optimize_glyph(struct aostk_glyph* glyph, uint8_t* data) {
	int pre_empty_bytes = 0;
	int pre_empty_rows = 0;
	int post_empty_rows = 0;
	int post_empty_bytes = 0;
	int pitch_optimal = (glyph->pitch+7) / 8;
	int i=0;
#ifndef TTF2C
	printf("Top: %d\n",glyph->top);
	printf("Height: %d pixels\n",glyph->size.height);
	printf("Width: %d pixels\n",glyph->size.width);
	printf("Unoptimized pitch: %d bytes\n",glyph->pitch);
	printf("Unoptimized size: %d bytes\n",glyph->size.height * glyph->pitch);
	printf("Optimal pitch: %d bytes\n", pitch_optimal);
#endif
	if (glyph->size.height == 0)
		return;

	// Find preceeding empty rows
	for (i=0; i<glyph->size.height * glyph->pitch; i++) {
		if (data[i] == 0)
			pre_empty_bytes++;
		else
			break;
	}

	pre_empty_rows = pre_empty_bytes/glyph->pitch;
#ifndef TTF2C
	printf("Pre empty bytes: %d\n", pre_empty_bytes);
	printf("Pre empty rows: %d\n", pre_empty_rows);
#endif

	// Count ending empty rows
	for (i=glyph->size.height * glyph->pitch - 1; i>=0; i--) {
			if (data[i] == 0)
			post_empty_bytes++;
		else
			break;
	}
	
	post_empty_rows = post_empty_bytes/glyph->pitch;
#ifndef TTF2C
	printf("Post empty bytes: %d\n", post_empty_bytes);
	printf("Post empty rows: %d\n", post_empty_rows);
#endif

	// Special for complete empty glyph
	if (pre_empty_rows + post_empty_rows > glyph->size.height) {
		post_empty_rows = glyph->size.height - 1;
		pre_empty_rows = 0;
	}

	// Move rows
	for (i=0; i<glyph->pitch * (glyph->size.height-pre_empty_rows); i++) {
		data[i] = data[glyph->pitch*pre_empty_rows + i];
	}

#ifndef TTF2C
	printf("New top should be: %d\n", pre_empty_rows);
#endif
	glyph->top -= pre_empty_bytes/glyph->pitch;
	glyph->size.height -= pre_empty_rows;
	glyph->size.height -= post_empty_rows;

// 	if (glyph->size.height<= 0)
// 		exit(0);
// 	if (glyph->size.height > 50)
// 		exit(0);
}

void my_draw_bitmap(const struct aostk_glyph* glyph) {
	const char* r;
	int rows = glyph->size.height;
	int pixel;
	int offset;
	unsigned int bit;
	int i;

	int rle_count = 0;
	int rle_last_bit = 0;
	
	r = glyph->data;
	while (rows--) {
		offset = 0;
		bit = 128;
		for (pixel=0; pixel<glyph->size.width; pixel++) {
			if (((*(r+offset)) & bit)) {
				if (rle_last_bit == 0) {
// 						printf("%d", rle_count);
					rle_last_bit = 1;
					rle_count = 0;
				}
				printf("%%");
			} else {

				if (rle_last_bit == 1) {
// 						printf("%d", rle_count);
					rle_last_bit = 0;
					rle_count = 0;
				}
				printf(".");
			}
			rle_count++;
			bit >>= 1;
			if (bit == 0) {
				bit = 128;
				offset ++;
			}
		}

		r += glyph->pitch;
		printf("\n");
	}
	printf("\n");
}

unsigned int unicode2iso8859_5(unsigned int unicode) {
	if (unicode >= 0x401 && unicode <= 0x045F)
		return unicode - 0x0360;

	return unicode;
}

char generate_glyph(FT_Face* face, int* height, unsigned int unicode) {
	int idx;
	int glyph_index = 0;
	int error;

// 	fprintf(stderr, "uni:%d\n", unicode);
	do {
		glyph_index = FT_Get_Char_Index( (*face), unicode);
		if (glyph_index == 0)
			face++;
	} while (*face && glyph_index==0);

	if (glyph_index == 0)
		face--;

// 	fprintf(stderr, "glyph:%d\n", glyph_index);

	if ((unicode != 0) && (glyph_index == 0)) {
#ifndef TTF2C
		printf("Glyph not found (%d)\n", unicode);
#endif
		return 0;
	}

	error = FT_Load_Glyph(
												(*face),          /* handle to face object */
						glyph_index,   /* glyph index           */
			/*load_flags*/ FT_LOAD_DEFAULT|FT_LOAD_RENDER|FT_LOAD_FORCE_AUTOHINT|FT_LOAD_TARGET_MONO);  /* load flags, see below */

	if (error) {
		fprintf(stderr,"Error loading\n");
		exit(0);
	}

	error = FT_Render_Glyph( (*face)->glyph,   /* glyph slot  */
													/*render_mode*/FT_RENDER_MODE_MONO ); /* render mode */

	if (error) {
		fprintf(stderr,"Error rendering %d\n", unicode);
		exit(0);
	}

	genglyphs[genfont.numglyphs].i            = unicode;
	genglyphs[genfont.numglyphs].size.width   = (*face)->glyph->bitmap.width;
	genglyphs[genfont.numglyphs].size.height  = (*face)->glyph->bitmap.rows;
	genglyphs[genfont.numglyphs].top          = (*face)->glyph->bitmap_top;
	genglyphs[genfont.numglyphs].left         = (*face)->glyph->bitmap_left;
	genglyphs[genfont.numglyphs].advance.x    = (*face)->glyph->advance.x >> 6;
	genglyphs[genfont.numglyphs].advance.y    = (*face)->glyph->advance.y >> 6;
	genglyphs[genfont.numglyphs].pitch        = (*face)->glyph->bitmap.pitch;

	if (*height < genglyphs[genfont.numglyphs].top)
		*height = genglyphs[genfont.numglyphs].top;

	int len = genglyphs[genfont.numglyphs].size.height * genglyphs[genfont.numglyphs].pitch;
	genglyphs[genfont.numglyphs].data      = malloc(len);

	memcpy((uint8_t*)genglyphs[genfont.numglyphs].data, (*face)->glyph->bitmap.buffer, len);

#ifndef TTF2C
	printf("Symbol #%d\n", genglyphs[genfont.numglyphs].i);
#endif
	optimize_glyph(&genglyphs[genfont.numglyphs], (uint8_t*)genglyphs[genfont.numglyphs].data);
#ifndef TTF2C
	my_draw_bitmap(&genglyphs[genfont.numglyphs]);
#endif

	genfont.numglyphs++;
	return 1;
}

int add_glyph_range(const char* rangename, FT_Face* faces, int* height, unsigned int from, unsigned int to) {
	unsigned int num = 0;
	unsigned int unicode;
	fprintf(stderr,"Adding %s: ",rangename);
	for (unicode = from; unicode<=to; unicode++)
		num += generate_glyph(faces, height, unicode) ? 1 : 0;
	fprintf(stderr," %d (%d) %s\n", to-from+1, num, num ?"":"!!!");
	return num;
}

int add_glyph_selection(const char* rangename, FT_Face* faces, int* height, unsigned int* unicodes, int count) {
	unsigned int num = 0;
	unsigned int unicode;
	fprintf(stderr,"Adding %s: ",rangename);
	int tmp = count;
	while (tmp--) {
// 		fprintf(stderr,"%04X\n", *unicodes);
		num += generate_glyph(faces, height, *unicodes) ? 1 : 0;
		unicodes++;
	}
	fprintf(stderr," %d (%d) %s\n", count, num, num ?"":"!!!");
	return num;
}

void initialize_face(FT_Face* face, FT_Library* library, int height, const char* filename) {
	int error;

	fprintf(stderr,"Loading %s... ", filename);
	error = FT_New_Face( *library, filename, 0, face );
	if ( error == FT_Err_Unknown_File_Format ) {
		fprintf(stderr,"Error opening font file\n");
		exit(1);
	} else
	if ( error ) {
		fprintf(stderr,"another error code means that the font file could not \nbe opened or read, or simply that it is broken...");
	}

	FT_CharMap  found = 0;
	int n;
	for (n = 0; n < (*face)->num_charmaps; n++ ) {
		FT_CharMap  charmap = (*face)->charmaps[n];
#ifndef TTF2C
		printf("platform_id:%d  encoding_id:%d\n", charmap->platform_id, charmap->encoding_id);
#endif
		found = charmap;
		if ( charmap->platform_id == 3/*my_platform_id*/ && charmap->encoding_id == 1/*my_encoding_id*/ ) {
// #ifndef TTF2C
// 			fprintf(stderr,"found charmap\n");
// #endif
			break;
		}
	}

	if ( !found ) {
		fprintf(stderr,"Encoding not found\n");
		exit(1);
	}

	/* now, select the charmap for the face object */
	error = FT_Set_Charmap( *face, found );
	if ( error ) {
		fprintf(stderr,"Error setting encoding\n");
		exit(1);
	}

	int width  = 0;

	error = FT_Set_Char_Size((*face), 0, height*64, 72, 72);

	error = FT_Set_Pixel_Sizes((*face), 0, height);

	if (error) {
		fprintf(stderr,"Error setting char size\n");
		exit(1);
	}
	
	fprintf(stderr,"ok\n");
}

FT_Face     faces[16];
const char* fontfiles[16];

int main(int argc, char* argv[]) {
	int error;
	int i;
	int j;
	const char* fontname = 0;
	FT_Library  library;

	if (argc < 4) {
		fprintf(stderr,"ttf2c <fontA.ttf> ... <fontB.ttf> <point-size> <name>\n");
		exit(1);
	}

	error = FT_Init_FreeType( &library );
	
	if ( error ) {
		fprintf(stderr,"Error initializing freetype\n");
		exit(1);
	}

	int height = 0;

	// Count the number of filenames
	// The argument is a font if it end with ".ttf"
	for (i=1; i<argc; i++) {
		const char* arg = strdup(argv[i]);
		const char* base = (const char*)basename(arg);
		if (strstr(base, ".ttf") == (base+strlen(base)-4)) {
			fontfiles[i-1] = argv[i];
		}
		free((void*)arg);

		// First non filename argument is the height
		if (fontfiles[i-1] == 0) {
			height = atoi(argv[i]);
			fontname = argv[i+1];
			break;
		}
	}

	i=0;
	while (fontfiles[i]) {
		initialize_face(&faces[i], &library, height, fontfiles[i]);
		i++;
	}
	
	height = 0;

	int num = 0;

	num += add_glyph_range("ASCII",          faces, &height, 0x0000, 0x00FF);
	num += add_glyph_range("Pi symbol",      faces, &height, 0x03C0, 0x03C0);
	num += add_glyph_range("Cyrillic",       faces, &height, 0x0400, 0x04FF);
	num += add_glyph_range("Arabic",         faces, &height, 0x0600, 0x06FF);
	num += add_glyph_range("Hindi",          faces, &height, 0x0900, 0x097F);
	num += add_glyph_range("Permille",       faces, &height, 0x2030, 0x2030);
	num += add_glyph_range("Degree",         faces, &height, 0x2070, 0x2070);
	num += add_glyph_range("Hiragana",       faces, &height, 0x3040, 0x309F);
	num += add_glyph_range("Katakana",       faces, &height, 0x30A0, 0x30FF);

	unsigned int cjk_selection[] = {
		0x4E0D,
		0x4F4D,
		0x52FE,
		0x5358,
		0x53CD,
		0x5B9A,
		0x5BFE,
		0x6574,
		0x65AD,
		0x660E,
		0x6E2C,
		0x793A,
		0x826F,
		0x8868,
		0x8A2D,
		0x8ABF,
		0x8EF8,
		0x914D,
		0x91CF,
		0x9577,
		0x9762,
		0x97F3,
		0x9AD8
	};
	num += add_glyph_selection("Selected CJK",   faces, &height, cjk_selection, sizeof(cjk_selection)/sizeof(cjk_selection[0]));

	num += add_glyph_range("Arabic Forms-A", faces, &height, 0xFB50, 0xFDFF);
	num += add_glyph_range("Arabic Forms-B", faces, &height, 0xFE70, 0xFEFF);
	num += add_glyph_range("Half/Fullwidth", faces, &height, 0xFF00, 0xFFEF);
	fprintf(stderr,"%d glyphs added\n", num);

	printf("// ");
	for (i=0; i<argc; i++)
		printf("%s ",argv[i]);
	printf("\n");
#ifdef TTF2C
	printf("#include \"ugui/ugui_font.h\"\n" );
#endif

	for (i=0; i<genfont.numglyphs; i++) {
#ifdef TTF2C
		printf("static const unsigned char sym_%d[] = {", genglyphs[i].i);
#endif
		for (j=0; j<genglyphs[i].size.height * genglyphs[i].pitch; j++) {
#ifdef TTF2C
			if (j>0)
				printf(",");
			printf("0x%02X", (uint8_t)genglyphs[i].data[j]);
#endif
		}
		free((void*)genglyphs[i].data);
		genglyphs[i].data = NULL;
#ifdef TTF2C
		printf("};\n");
#endif
	}
	
#ifdef TTF2C
	printf("\nstatic const struct aostk_glyph glyphs[] = {\n");
#endif

	for (i=0; i<genfont.numglyphs; i++) {
#ifdef TTF2C
		if (i>0)
			printf(",\n");
		printf("{ /* %d */  i: %d, size: {  width: %d, height: %d} ,top: %d, advance: { x: %d, y: %d },  pitch: %d, data: sym_%d, left: %d}", genglyphs[i].i, genglyphs[i].i, genglyphs[i].size.width, genglyphs[i].size.height, genglyphs[i].top, genglyphs[i].advance.x, genglyphs[i].advance.y, genglyphs[i].pitch, genglyphs[i].i, genglyphs[i].left);
#endif
	}

#ifdef TTF2C
	printf("\n};\n");
#endif

// 	printf("struct aostk_font %s = { .numglyphs = %d, .height = %d, .glyphs = glyphs };\n", argv[3], genfont.numglyphs, height);
#ifdef TTF2C
	printf("extern const struct aostk_font %s;\n", fontname);
	printf("const struct aostk_font %s = { numglyphs : %d, height : %d, glyphs };\n", fontname, genfont.numglyphs, height);
#endif
// 		for (i=0; i<verafont.numglyphs; i++) {
// 			aostk_ttf_raster(&verafont, 0, 0, verafont.glyphs[i].i, 0);
// 		}
	return 0;
}
