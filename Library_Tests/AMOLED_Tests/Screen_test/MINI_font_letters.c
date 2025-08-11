/*******************************************************************************
 * Size: 29 px
 * Bpp: 1
 * Opts: --bpp 1 --size 29 --no-compress --stride 1 --align 1 --font MINI-Font.ttf --range 67,73,80,83,103 --format lvgl -o MINI_font_letters.c
 ******************************************************************************/

#include <lvgl.h>

#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif


#ifndef MINI_FONT_LETTERS
#define MINI_FONT_LETTERS 1
#endif

#if MINI_FONT_LETTERS

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0043 "C" */
    0x7f, 0xdf, 0xff, 0xff, 0xff, 0xff, 0x9f, 0xe3,
    0xfc, 0x7f, 0x8f, 0xf1, 0xfe, 0x3, 0xc0, 0x78,
    0xf, 0x1, 0xe0, 0x3c, 0x7, 0x80, 0xf0, 0x1e,
    0x3f, 0xc7, 0xf8, 0xff, 0x1f, 0xe3, 0xfc, 0x7f,
    0x8f, 0xf1, 0xff, 0xff, 0xff, 0xff, 0xf7, 0xfc,

    /* U+0049 "I" */
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0,

    /* U+0050 "P" */
    0xff, 0xdf, 0xff, 0xff, 0xf8, 0xff, 0x1f, 0xe3,
    0xfc, 0x7f, 0x8f, 0xf1, 0xfe, 0x3f, 0xc7, 0xf8,
    0xff, 0x1f, 0xe3, 0xfc, 0x7f, 0xff, 0xff, 0xff,
    0xfb, 0xc0, 0x78, 0xf, 0x1, 0xe0, 0x3c, 0x7,
    0x80, 0xf0, 0x1e, 0x3, 0xc0, 0x78, 0xf, 0x0,

    /* U+0053 "S" */
    0x7f, 0xdf, 0xff, 0xff, 0xfd, 0xff, 0x1f, 0xe3,
    0xfc, 0x7f, 0x8f, 0xf1, 0xfe, 0x3, 0xe0, 0x7f,
    0x7, 0xf0, 0x7f, 0x7, 0xf0, 0x7f, 0x3, 0xe0,
    0x7f, 0xc7, 0xf8, 0xff, 0x1f, 0xe3, 0xfc, 0x7f,
    0x8f, 0xf1, 0xff, 0x7f, 0xff, 0xff, 0xf7, 0xfc,

    /* U+0067 "g" */
    0x7f, 0xff, 0xff, 0xff, 0xf8, 0xff, 0x1f, 0xe3,
    0xfc, 0x7f, 0x8f, 0xf1, 0xfe, 0x3f, 0xc7, 0xf8,
    0xff, 0xff, 0xff, 0xdf, 0xf8, 0xf, 0x1, 0xe0,
    0x3c, 0x7, 0x80, 0xf0, 0x1e, 0x3, 0xc0, 0x7f,
    0x8f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf7, 0xfc
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 464, .box_w = 11, .box_h = 29, .ofs_x = 9, .ofs_y = 0},
    {.bitmap_index = 40, .adv_w = 464, .box_w = 4, .box_h = 29, .ofs_x = 13, .ofs_y = 0},
    {.bitmap_index = 55, .adv_w = 464, .box_w = 11, .box_h = 29, .ofs_x = 9, .ofs_y = 0},
    {.bitmap_index = 95, .adv_w = 464, .box_w = 11, .box_h = 29, .ofs_x = 9, .ofs_y = 0},
    {.bitmap_index = 135, .adv_w = 464, .box_w = 11, .box_h = 29, .ofs_x = 9, .ofs_y = 0}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/

static const uint16_t unicode_list_0[] = {
    0x0, 0x6, 0xd, 0x10, 0x24
};

/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 67, .range_length = 37, .glyph_id_start = 1,
        .unicode_list = unicode_list_0, .glyph_id_ofs_list = NULL, .list_length = 5, .type = LV_FONT_FMT_TXT_CMAP_SPARSE_TINY
    }
};



/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LVGL_VERSION_MAJOR == 8
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
#endif

#if LVGL_VERSION_MAJOR >= 8
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = NULL,
    .kern_scale = 0,
    .cmap_num = 1,
    .bpp = 1,
    .kern_classes = 0,
    .bitmap_format = 0,
#if LVGL_VERSION_MAJOR == 8
    .cache = &cache
#endif

};



/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LVGL_VERSION_MAJOR >= 8
const lv_font_t MINI_font_letters = {
#else
lv_font_t MINI_font_letters = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 29,          /*The maximum line height required by the font*/
    .base_line = 0,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = 0,
    .underline_thickness = 0,
#endif
    // .static_bitmap = 0,
    .dsc = &font_dsc,          /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
#if LV_VERSION_CHECK(8, 2, 0) || LVGL_VERSION_MAJOR >= 9
    .fallback = NULL,
#endif
    .user_data = NULL,
};



#endif /*#if MINI_FONT_LETTERS*/
