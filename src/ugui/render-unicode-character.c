/*
 * Program to render a single glyph from a TTF font and output it as
 * a PGM (greyscale) image.  Useful for examining a symbol up close,
 * and provides a simple example of using the Freetype library.
 *
 * Usage:
 *   render-unicode-character -f FONT-FILE CHAR-CODE > out.pgm
 *
 * CHAR-CODE is a decimal or hexadecimal (with 0x prefix) Unicode character
 * code for the symbol to render.  FONT-FILE is the full path and filename
 * of a TrueType font containing the appropriate symbol.
 *
 * (c) Copyright 2004 Geoff Richards.
 * You may do what you want with this program.  There is no warranty.
 * If you like it, buy me beer.
 *
 * http://ungwe.org/blog/2004/02/05/22:11/
 */

#include <ft2build.h>
#include FT_FREETYPE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* Stuff we get from the arguments. */
static const char *progname;
static const char *font_filename = 0;
static FT_F26Dot6 point_size = 72;
static FT_UInt res_dpi = 600;
static int border = 10;

void draw_bitmap (const FT_Bitmap* bitmap);
int safe_atoi (const char *s, const char *what_for);
int decode_arguments (int argc, char **argv);


int
main (int argc, char **argv)
{
    int i, char_code;
    FT_UInt glyph_index;
    FT_Error err;
    FT_Library library;
    FT_Face face;

    i = decode_arguments(argc, argv);
    char_code = safe_atoi(argv[i], "character code");

    err = FT_Init_FreeType(&library);
    if (err) {
        fprintf(stderr, "%s: error initialising libfreetype.\n", progname);
        return 2;
    }

    err = FT_New_Face(library, font_filename, 0, &face);
    if (err == FT_Err_Unknown_File_Format) {
        fprintf(stderr, "%s: font file '%s' can't be read (might be"
                " unsupported).\n", progname, font_filename);
        return 2;
    }
    else if (err) {
        fprintf(stderr, "%s: error reading font file '%s'.\n",
                progname, font_filename);
        return 2;
    }

    err = FT_Set_Char_Size(face, 0, point_size * 64, res_dpi, res_dpi);
    if (err) {
        fprintf(stderr, "%s: error setting font size.\n", progname);
        return 2;
    }

    glyph_index = FT_Get_Char_Index(face, char_code);
    if (!glyph_index) {
        fprintf(stderr, "%s: character %d not found in font.\n",
                progname, char_code);
        return 3;
    }

    err = FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
    if (err) {
        fprintf(stderr, "%s: error loading glyph.\n", progname);
        return 2;
    }

    err = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
    if (err) {
        fprintf(stderr, "%s: error rendering glyph.\n", progname);
        return 2;
    }

    draw_bitmap(&face->glyph->bitmap);

    err = FT_Done_FreeType(library);
    if (err) {
        fprintf(stderr, "%s: error cleaning up after libfreetype.\n", progname);
        return 2;
    }

    return 0;
}


void
draw_bitmap (const FT_Bitmap* bitmap)
{
    unsigned char *img;
    FT_Int i, x, y;
    FT_Int width = bitmap->width,
           height = bitmap->rows;
    FT_Int img_width = width + border * 2,
           img_height = height + border * 2;

    img = malloc(img_width * img_height);
    if (!img) {
        fprintf(stderr, "%s: error allocating memory for image.\n", progname);
        exit(2);
    }

    /* Initialise to black */
    for (i = 0; i < img_width * img_height; ++i)
        img[i] = 0;
            

    /* Copy glyph bitmap into image */
    for (y = 0; y < height; ++y)
        for (x = 0; x < width; ++x)
            img[border + x + (border + y) * img_width] =
                bitmap->buffer[x + y * bitmap->width];

    /* Write a PGM file. */
    printf("P5\n%d %d\n255\n", img_width, img_height);
    for (y = 0; y < img_height; ++y)
        for (x = 0; x < img_width; ++x)
            putchar(img[x + y * img_width]);

    free(img);
}


int
safe_atoi (const char *s, const char *what_for)
{
    char *endptr;
    int i = strtol(s, &endptr, 0);

    if (endptr == s || *endptr) {
        fprintf(stderr, "%s: bad number '%s' for %s.\n",
                progname, s, what_for);
        exit(1);
    }
    else if (i < 0) {
        fprintf(stderr, "%s: negative numbers for %s are not allowed.\n",
                progname, what_for);
        exit(1);
    }

    return i;
}


int
decode_arguments (int argc, char **argv)
{
    int i = 1;
    char c;
    progname = argv[0];

    while (i < argc && argv[i][0] == '-') {
        if (!argv[i][1] || argv[i][2]) {
            fprintf(stderr, "%s: bad option '%s'.\n", progname, argv[i]);
            exit(1);
        }
        if (i == argc - 1) {
            fprintf(stderr, "%s: missing value for option '%s'.\n",
                    progname, argv[i]);
            exit(1);
        }

        c = argv[i++][1];
        if (c == 'f')
            font_filename = argv[i];
        else if (c == 's')
            point_size = safe_atoi(argv[i], "point size");
        else if (c == 'r')
            res_dpi = safe_atoi(argv[i], "resolution");
        else if (c == 'b')
            border = safe_atoi(argv[i], "border width");
        else {
            fprintf(stderr, "%s: unknown option '-%c'.\n", progname, c);
            exit(1);
        }

        ++i;
    }

    if (i >= argc) {
        fprintf(stderr, "%s: character code argument required\n", progname);
        exit(1);
    }

    if (!font_filename) {
        fprintf(stderr, "%s: font filename required (-f option)\n", progname);
        exit(1);
    }

    return i;
}

/* vi:set ts=4 sw=4 expandtab: */
