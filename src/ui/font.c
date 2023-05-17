#include "soloscuro/font.h"

#include <gff/gui.h>

/*
extern int soloscuro_font_load(gff_manager_t *man, char **data, int32_t *w, int32_t *h, const uint32_t fg_color, const uint32_t bg_color) {
    gff_font_t *font;
    gff_char_t *ds_char;
    uint8_t    *pixel_idx;
    uint8_t    *img, *img_row;
    int32_t     transparent = 0x00000000;
    int32_t     total_width = 0, current_width = 0;
    int32_t     current_column = 0;

    //unsigned char* data = gff_create_font_rgba(man->ds1.resource, 'A', 0x00000000, 0xFFFFFFFF);
    if (gff_read_font(man->ds1.resource, 100, &font)) {
        goto read_error;
    }

    for (int c = 0; c < font->num; c++) {
        ds_char = (gff_char_t*)(((uint8_t*)font) + *(font->char_offset + c));
        total_width += ds_char->width;
    }

    *data = img = calloc(1, 4 * font->height * total_width);
    *w = total_width;
    *h = font->height;

    //printf("fg_color = 0x%x, bg_color = 0x%x\n", fg_color, bg_color);
    int true_fg_color = int_byte_swap(fg_color);
    int true_bg_color = int_byte_swap(bg_color);
    for (int c = 0; c < font->num; c++) {
        ds_char = (gff_char_t*)(((uint8_t*)font) + *(font->char_offset + c));
        if (ds_char->width < 1) { continue; }

        pixel_idx = ds_char->data;

        //printf("c(%d:'%c'): w:%d h%d\n", c, ds_char->width ? c : 0, ds_char->width, font->height);
        img_row = img;
        current_column = 4 * current_width + 4 * total_width * (font->height - 1);
        for (int x = 0; x < font->height; x++) {
            img_row = img + current_column;  
            for (int y = 0; y < ds_char->width; y++, pixel_idx++) {
                float intensity = font->colors[*pixel_idx] / 255.0;
                if (intensity <= 0.0001) {
                    *(int*)(img_row + 4*y) = transparent;
                    continue;
                }
                unsigned char *cp = (unsigned char*)(img_row + 4*y);
                unsigned char *fcp = (unsigned char*)&true_fg_color;
                unsigned char *bcp = (unsigned char*)&true_bg_color;
                *(cp + 0) = *(fcp + 0) * intensity;
                *(cp + 1) = *(fcp + 1) * intensity;
                *(cp + 2) = *(fcp + 2) * intensity;
                *(cp + 3) = *(fcp + 3) * intensity;
                *(cp + 0) += *(bcp + 0) * (1.0 - intensity);
                *(cp + 1) += *(bcp + 1) * (1.0 - intensity);
                *(cp + 2) += *(bcp + 2) * (1.0 - intensity);
                *(cp + 3) += *(bcp + 3) * (1.0 - intensity);
            }
            current_column -= 4 * total_width;
        }
        current_width += ds_char->width;
    }

    free(font);

read_error:
    return EXIT_FAILURE;
}
*/
