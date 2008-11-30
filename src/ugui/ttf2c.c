#include <ft2build.h>
#include FT_FREETYPE_H

#include <stdio.h>
#include <stdint.h>

#include "ugui/ugui_font.h"

#define TTF2C

#define MAX_GLYPHS 1000

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

	#warning optimize pitch here

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


int main(int argc, char* argv[]) {
	int error;
	int i;
		int j;
	FT_Library  library;
	FT_Face     face;
	int glyph_index;

	if (argc < 4) {
		printf("ttf2c <file.ttf> <point-size> <name>\n");
		exit(1);
	}

	error = FT_Init_FreeType( &library );
	
	if ( error ) {
		printf("Error initializing freetype\n");
		exit(1);
	}
	
	//error = FT_New_Face( library,"VeraMono.ttf", 0, &face );s
	error = FT_New_Face( library, argv[1], 0, &face );
	//error = FT_New_Face( library,"FreeMonoBold.ttf", 0, &face );
	if ( error == FT_Err_Unknown_File_Format )
	{
		printf("Error opening font file\n");
		exit(1);
	} else 
	if ( error ) {
		printf("another error code means that the font file could not \nbe opened or read, or simply that it is broken...");
	}
	
	FT_CharMap  found = 0;
	FT_CharMap  charmap;
	int n;
	for (n = 0; n < face->num_charmaps; n++ ) {
		charmap = face->charmaps[n];
#ifndef TTF2C
		printf("platform_id:%d  encoding_id:%d\n", charmap->platform_id, charmap->encoding_id);
#endif
		if ( charmap->platform_id == 3/*my_platform_id*/ && charmap->encoding_id == 1/*my_encoding_id*/ ) {
			found = charmap;
#ifndef TTF2C
			printf("found charmap\n");
#endif
			break;
		}
	}

	if ( !found ) {
		printf("Encoding not found\n");
		exit(1);
	}

	/* now, select the charmap for the face object */
	error = FT_Set_Charmap( face, found );
	if ( error ) {
		printf("Error setting encoding\n");
		exit(1);
	}

	int width  = 0;
	int height = atoi(argv[2]);
	
	error = FT_Set_Char_Size(
		face,    /* handle to face object           */
		0,   /* char_width in 1/64th of points  */
		height*64,   /* char_height in 1/64th of points */
		72,     /* horizontal device resolution    */
		72 );   /* vertical device resolution      */
	
	error = FT_Set_Pixel_Sizes(
		face,   /* handle to face object */
		0,      /* pixel_width           */
		height );   /* pixel_height          */
	
	if (error) {
		//printf("Error setting char size\n");
		//exit(0);
	}

	height = 0;
	
// 	printf("max_advanc_height %d\n", face->max_advance_height);
// 	printf("units_per_EM %d\n", face->units_per_EM);


// 	printf("EM size %d\n", 4 * 300/72);
//
	int idx;
	unsigned int glyph_list[] = {
		0x3C0,  // Pi
		0x2030, // Permille
		0x2070, // Degree
		0};
	for (idx=0x0; idx<=MAX_GLYPHS; idx++) {
		int len;
// 		if (idx == '_'+1)
// 			break;
// 		idx = '_';
		unsigned int unicode = idx>=256 ? glyph_list[idx - 256] : idx;
// 		unicode = glyph_list[idx];
		if (idx > 0 && unicode ==0)
			break;
		glyph_index = FT_Get_Char_Index( face, unicode);
	
		if ((unicode != 0) && (glyph_index == 0)) {
#ifndef TTF2C
			printf("Glyph not found (%d)\n", unicode);
#endif
			continue;
// 			exit(1);
		}	
		
		error = FT_Load_Glyph(
													face,          /* handle to face object */
							glyph_index,   /* glyph index           */
				/*load_flags*/ FT_LOAD_DEFAULT|FT_LOAD_RENDER|FT_LOAD_FORCE_AUTOHINT|FT_LOAD_TARGET_MONO);  /* load flags, see below */
		
		if (error) {
			printf("Error loading\n");
			exit(0);
		}
		
		error = FT_Render_Glyph( face->glyph,   /* glyph slot  */
														/*render_mode*/FT_RENDER_MODE_MONO ); /* render mode */
		
		if (error) {
			printf("Error rendering %d\n", unicode);
 			exit(0);
			continue;
		}
		
		genglyphs[genfont.numglyphs].i            = unicode;
		genglyphs[genfont.numglyphs].size.width   = face->glyph->bitmap.width;
		genglyphs[genfont.numglyphs].size.height  = face->glyph->bitmap.rows;
		genglyphs[genfont.numglyphs].top          = face->glyph->bitmap_top;
		genglyphs[genfont.numglyphs].left         = face->glyph->bitmap_left;
		genglyphs[genfont.numglyphs].advance.x    = face->glyph->advance.x >> 6;
		genglyphs[genfont.numglyphs].advance.y    = face->glyph->advance.y >> 6;
		genglyphs[genfont.numglyphs].pitch        = face->glyph->bitmap.pitch;

		if (height < genglyphs[genfont.numglyphs].top)
			height = genglyphs[genfont.numglyphs].top;

		len = genglyphs[genfont.numglyphs].size.height * genglyphs[genfont.numglyphs].pitch;
		genglyphs[genfont.numglyphs].data      = malloc(len);
		
		memcpy((uint8_t*)genglyphs[genfont.numglyphs].data, face->glyph->bitmap.buffer, len);

#ifndef TTF2C
		printf("Symbol #%d\n", genglyphs[genfont.numglyphs].i);
#endif
		optimize_glyph(&genglyphs[genfont.numglyphs], genglyphs[genfont.numglyphs].data);
#ifndef TTF2C
		my_draw_bitmap(&genglyphs[genfont.numglyphs]); 
#endif

		genfont.numglyphs++;
	}

#ifdef TTF2C
	printf(" #include \"ugui/ugui_font.h\"\n" );
#endif

	for (i=0; i<genfont.numglyphs; i++) {
#ifdef TTF2C
		printf("static const char sym_%d[] = {", genglyphs[i].i);
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
	printf("/*const*/ struct aostk_font %s = { numglyphs : %d, height : %d, glyphs };\n", argv[3], genfont.numglyphs, height);
#endif
// 		for (i=0; i<verafont.numglyphs; i++) {
// 			aostk_ttf_raster(&verafont, 0, 0, verafont.glyphs[i].i, 0);
// 		}

}
