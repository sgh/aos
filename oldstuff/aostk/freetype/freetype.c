#include <ft2build.h>
#include FT_FREETYPE_H

#include <stdio.h>
#include <stdint.h>

#include <aostk_font.h>

struct aostk_glyph genglyphs[1000];

struct aostk_font genfont = {
	.numglyphs = 0,
 	.glyphs = genglyphs,
};



void my_draw_bitmap(const struct aostk_glyph* glyph) {
	const char* r;
	int rows = glyph->size.height;
	int pixel;
	int offset;
	unsigned int bit;
	int i;
	
 	r = glyph->data;
 	while (rows--) {
 		offset = 0;
 		bit = 128;
 		for (pixel=0; pixel<glyph->size.width; pixel++) {
 			printf("%c", ((*(r+offset)) & bit) ? '%' : '.');
 			//printf("%d\n", bit);
 			bit >>= 1;
 			if (bit == 0) {
 				bit = 128;
 				offset ++;
 			}
 		}
 		
 // 		for (i=0; i<bitmap->pitch; i++)
 // 			printf("%02X ", *(r+i));
 		
 		r += glyph->pitch;
 		printf("\n");
	}
 		printf("\n");

/*	printf("static const char const glyph_%d_data[] = {", ch);
	for (i=0; i<(bitmap->pitch*bitmap->rows); i++) {
		if (i!=0) printf(",");
		printf("0x%0X", bitmap->buffer[i]);
	}
	printf("};\n");

	printf("static struct aostk_ttffont glyph_%d = {.ch = %d, .height = %d, .width = %d, .pitch = %d, .left = %d, .top = %d, .data = glyph_%d_data};\n", ch, ch, bitmap->rows, bitmap->width, bitmap->pitch, left, top, ch);
// 	printf(" .data = {");*/
	
}


void aostk_ttf_raster(struct aostk_font* f, unsigned int posx, unsigned int posy, unsigned char c, int scanlines) {
	unsigned int x;
	unsigned int y;
	unsigned int i;
	const struct aostk_glyph* sym = NULL;

	for (i=0; i<f->numglyphs; i++) {
		if (f->glyphs[i].i == c) {
			sym = &f->glyphs[i];
			break;
		}
	}
	
	if (sym == NULL)
		return;

	my_draw_bitmap(sym);
// 	for (y=0; y<f->height; y++) {
// 		if (scanlines < 1)
// 			break;
// 		
// 		for (x=0; x<f->width; x++) {
// 			if (sym->data[y] & (1<<x))
// 				aostk_putpixel(posx+x,posy+y,0xf);
// 		}
// 		scanlines--;
// 	}
}

struct aostk_font verafont;

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
	
// 	error = FT_Select_CharMap( face, FT_ENCODING_ADOBE_LATIN_1);
// 	if (error) {
// 		printf("Error setting charmap\n");
// 		exit(1);
// 	}


	int width  = atoi(argv[2]);
	int height = atoi(argv[2]);
	
	error = FT_Set_Char_Size(
		face,    /* handle to face object           */
		width*64,   /* char_width in 1/64th of points  */
		height*64,   /* char_height in 1/64th of points */
		72,     /* horizontal device resolution    */
		72 );   /* vertical device resolution      */
	
// 	error = FT_Set_Pixel_Sizes(
// 		face,   /* handle to face object */
// 		30,      /* pixel_width           */
// 		20 );   /* pixel_height          */
	
	if (error) {
		printf("Error setting char size\n");
		exit(0);
	}
	
// 	printf("max_advanc_height %d\n", face->max_advance_height);
// 	printf("units_per_EM %d\n", face->units_per_EM);


// 	printf("EM size %d\n", 4 * 300/72);


	for (i=0x0; i<=0xFF; i++) {
		int len;
		glyph_index = FT_Get_Char_Index( face, i );
		
		
		error = FT_Load_Glyph(
													face,          /* handle to face object */
							glyph_index,   /* glyph index           */
				/*load_flags*/ FT_LOAD_DEFAULT);  /* load flags, see below */
		
		if (error) {
			printf("Error loading\n");
			exit(0);
		}
		
		error = FT_Render_Glyph( face->glyph,   /* glyph slot  */
														/*render_mode*/FT_RENDER_MODE_MONO ); /* render mode */
		
		if (error) {
			printf("Error rendering %d\n", i);
// 			exit(0);
			continue;
		}
		
		

		genglyphs[genfont.numglyphs].i            = i;
		genglyphs[genfont.numglyphs].size.width   = face->glyph->bitmap.width;
		genglyphs[genfont.numglyphs].size.height  = face->glyph->bitmap.rows;
		genglyphs[genfont.numglyphs].top          = face->glyph->bitmap_top;
		genglyphs[genfont.numglyphs].left         = face->glyph->bitmap_left;
		genglyphs[genfont.numglyphs].advance.x    = face->glyph->advance.x >> 6;
		genglyphs[genfont.numglyphs].advance.y    = face->glyph->advance.y >> 6;
		genglyphs[genfont.numglyphs].pitch        = face->glyph->bitmap.pitch;

		len = genglyphs[i].size.height * genglyphs[i].pitch;
		genglyphs[genfont.numglyphs].data      = malloc(len);;
		
		memcpy((uint8_t*)genglyphs[genfont.numglyphs].data, face->glyph->bitmap.buffer, len);
		
		//my_draw_bitmap(&genglyphs[genfont.numglyphs]);

		genfont.numglyphs++;
	}


	printf(" #include <aostk_font.h>\n" );


	for (i=0; i<genfont.numglyphs; i++) {
		printf("static const char const sym_%d[] = {", i);
		for (j=0; j<genglyphs[i].size.height * genglyphs[i].pitch; j++) {
			if (j>0)
				printf(",");
			printf("0x%02X", (uint8_t)genglyphs[i].data[j]);
		}
		free((void*)genglyphs[i].data);
		genglyphs[i].data = NULL;
		printf("};\n");
	}
	
	printf("\nstatic const struct aostk_glyph const glyphs[] = {\n");

	for (i=0; i<genfont.numglyphs; i++) {
		if (i>0)
				printf(",\n");
			printf("{ /* %d */  .i = %d, .size = { .width = %d, .height = %d} , .top = %d, .advance = { .x = %d, .y = %d }, .pitch = %d, .data = sym_%d, .left = %d}", genglyphs[i].i, genglyphs[i].i, genglyphs[i].size.width, genglyphs[i].size.height, genglyphs[i].top, genglyphs[i].advance.x, genglyphs[i].advance.y, genglyphs[i].pitch, i, genglyphs[i].left);
			
	}

	
	printf("\n};\n");

	printf("struct aostk_font %s = { .numglyphs = %d, .height = %d, .glyphs = glyphs };\n", argv[3], genfont.numglyphs, height);

		for (i=0; i<verafont.numglyphs; i++) {
			aostk_ttf_raster(&verafont, 0, 0, verafont.glyphs[i].i, 0);
		}

}
