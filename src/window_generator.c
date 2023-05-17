#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "soloscuro.h"
#include "soloscuro/button.h"
#include "soloscuro/window.h"

#include <gff/gff.h>
#include <gff/gfftypes.h>
#include <gff/gui.h>
#include <gff/image.h>
#include <gff/manager.h>

#include "orx.h"

typedef struct print_args_s {
    gff_game_type_t game;
    gff_manager_t  *man;
    gff_window_t   *win;
    gff_gui_item_t *item;
    union {
        gff_button_t *button;
    } data;
    uint16_t button_num, ebox_num;
    char *config;
    size_t pos, len;
} print_args_t;

static int print_window(print_args_t *args);
static int print_button(print_args_t *args);
static int print_button_arg(print_args_t *args, gff_button_t *button, int gff_type);

static int append_config(print_args_t *args, const char *fmt, ...) {
    size_t amt;
    va_list cargs;

    va_start(cargs, fmt);

    amt = vsnprintf(args->config + args->pos, args->len - args->pos - 1,
            fmt, cargs
    );

    va_end(cargs);

    if (amt + args->pos >= args->len) {
        goto overflow;
    }

    args->pos += amt;

    return EXIT_SUCCESS;
overflow:
    return EXIT_FAILURE;
}

static int load_button_from_gff(gff_manager_t *man, const int res_id, char *config, size_t *config_pos) {
    char buf[4096];
    gff_button_t *gff_button;
    gff_chunk_header_t chunk;

    if (gff_find_chunk_header(man->ds1.resource, &chunk, GFF_BUTN, res_id)) {
        goto no_header;
    }

    if (gff_read_chunk(man->ds1.resource, &chunk, buf, 4096) <= 0) {
        goto read_error;
    }

    gff_button = (gff_button_t*)buf;
    printf("id: %d, icon:%d, w:%d, h:%d, x:%d, y:%d",
            gff_button->rh.id,
            gff_button->icon_id,
            gff_button->frame.width,
            gff_button->frame.height,
            gff_button->iconx,
            gff_button->icony
            );

    // palette is r0
    return EXIT_SUCCESS;

read_error:
no_header:
    return EXIT_FAILURE;
}

typedef struct gff_accl_entry_s {
    uint8_t  flags;
    uint16_t event;
    uint16_t user_id;
} gff_accl_entry_t;

typedef struct gff_accl_s {
    gff_resource_header_t rh;
    uint16_t              count;
    gff_accl_entry_t      entries[];
} gff_accl_t;

static int load_accl_from_gff(gff_manager_t *man, const int res_id) {
    gff_chunk_header_t chunk;
    gff_accl_t        *accl;
    int                amt;
    char               buf[1024];

    if (gff_find_chunk_header(man->ds1.resource, &chunk, GFF_ACCL, res_id)) {
        goto no_header;
    }

    //printf("chunk.length = %d, %d\n", chunk.length, sizeof(gff_accl_t));
    //if ((amt = gff_read_chunk(man->ds1.resource, &chunk, &accl, sizeof(gff_accl_t))) < sizeof(gff_accl_t)) {
    if ((amt = gff_read_chunk(man->ds1.resource, &chunk, buf, 1024)) < sizeof(gff_accl_t)) {
        //printf("amt = %d\n", amt);
        goto read_header_error;
    }

    accl = (gff_accl_t*)buf;
    printf("ACCL: id: %d, type: %d, len: %d, count:%d\n", accl->rh.id, accl->rh.type, accl->rh.len, accl->count);
    for (int i = 0; i < accl->count; i++) {
        printf("    %d: flags: 0x%x event:%d id: %d\n", i, accl->entries[i].flags, accl->entries[i].event, accl->entries[i].user_id);
    }
    //printf("'%c%c%c%c'\n", buf[0], buf[1], buf[2], buf[3]);

    return EXIT_FAILURE;
read_header_error:
no_header:
    printf("ACCL: error!\n");
    return EXIT_FAILURE;

}

static int print_ebox(print_args_t *args, gff_gui_item_t *item) {
    char buf[4096];
    gff_chunk_header_t chunk;
    gff_ebox_t      *ebox;
    int amt;
    int frames;
   
    if (gff_find_chunk_header(args->man->ds1.resource, &chunk, GFF_EBOX, item->id)) {
        goto no_header;
    }

    if ((amt = gff_read_chunk(args->man->ds1.resource, &chunk, buf, 4096)) <= 0) {
        goto read_error;
    }

    ebox = (gff_ebox_t*) buf;

    frames = gff_get_frame_count(args->man->ds1.resource, GFF_BMP, ebox->frame.border_bmp);

    printf("ebox(%d): max_lines: %d, styles: %d, runs: %d, buf_size: %d, user_id: %d"
            " w:%d, h:%d, border: %d, "
            //"(%d, %d -> %d, %d)" "\n"
            ,
            item->id, ebox->max_lines, ebox->styles, ebox->runs, ebox->size, ebox->user_id,
            ebox->frame.bounds.xmax, ebox->frame.bounds.ymax, ebox->frame.border_bmp
            //ebox->rect.xmin, ebox->rect.ymin, ebox->rect.xmax, ebox->rect.ymax
          );
    printf("(%d, %d -> %d, %d)\n",
        item->init_bounds.xmin, item->init_bounds.ymin, item->init_bounds.xmax, item->init_bounds.ymax
    );

    append_config(args,
           "[Window%dBox%dBackground]\n"
           "Graphic = Window%dBox%dTex0\n"
           "Position = (%d, %d, 0)\n\n",
           args->win->rh.id, args->ebox_num,
           args->win->rh.id, args->ebox_num,
           item->init_bounds.xmin, item->init_bounds.ymin
    );

    append_config(args,
           "[Window%dBox%dTextText]\n"
           "String = \"Test! THe quick brown fox jumps over the lazy \ndog.\"\n"
           "Font = %s\n\n",
           args->win->rh.id, args->ebox_num,
           soloscuro_ds1ebox_font(args->win->rh.id, args->ebox_num)
           );

    append_config(args,
           "[Window%dBox%dTextGraphic]\n"
           "Text  = Window%dBox%dTextText\n"
           "Pivot = top left\n\n",
           args->win->rh.id, args->ebox_num,
           args->win->rh.id, args->ebox_num
           );

    append_config(args,
           "[Window%dBox%dText]\n"
           "Graphic = Window%dBox%dTextGraphic\n"
           "Size     = (%d, %d, 0)\n"
           "Position = (%d, %d, 0)\n\n",
           args->win->rh.id, args->ebox_num,
           args->win->rh.id, args->ebox_num,
           ebox->frame.bounds.xmax,
           10 * ebox->max_lines,
           item->init_bounds.xmin, item->init_bounds.ymin
           );

    append_config(args,
           "[Window%dBox%d@DS1EBox]\n"
           "ChildList = Window%dBox%dText # Window%dBox%dBackground\n",
           args->win->rh.id, args->ebox_num,
           args->win->rh.id, args->ebox_num,
           args->win->rh.id, args->ebox_num
    );

    append_config(args, "\n");

    for (int i = 0; i < frames; i++) {
        append_config(args,
               "[Window%dBox%dTex%d]\n"
               "Texture  = ds1.resource.%d.%d.r0.%s\n"
               "\n",
               args->win->rh.id, args->button_num, i,
               ebox->frame.border_bmp, i,
               "bmp"
        );
    }

    args->ebox_num++;

overflow:
read_error:
no_header:
    return EXIT_FAILURE;
}

static int load_frame_from_gff(gff_manager_t *man, const int res_id) {
    char buf[4096];
    gff_chunk_header_t chunk;
    gff_app_frame_t *gff_frame;
    int amt;
   
    if (gff_find_chunk_header(man->ds1.resource, &chunk, GFF_APFM, res_id)) {
        goto no_header;
    }

    if ((amt = gff_read_chunk(man->ds1.resource, &chunk, buf, 4096)) <= 0) {
        goto read_error;
    }

    gff_frame = (gff_app_frame_t*)buf;
    printf(" id: %d, w: %d, h: %d, bmp: %d/%d, flags = 0x%x, type = 0x%x "
            " snapMode = %d, event = 0x%x"
            " (%d, %d -> %d, %d)\n",
            gff_frame->rh.id,
            gff_frame->frame.width,
            gff_frame->frame.height,
            gff_frame->frame.background_bmp,
            gff_frame->frame.border_bmp,
            gff_frame->frame.flags,
            gff_frame->frame.type,
            gff_frame->snapMode,
            gff_frame->event_filter,
            gff_frame->frame.zonebounds.xmin, gff_frame->frame.zonebounds.ymin,
            gff_frame->frame.zonebounds.xmax, gff_frame->frame.zonebounds.ymax
            );
    char name[128];
    sprintf(buf, "apfm%d.dat", gff_frame->rh.id);
    FILE *file = fopen(buf, "wb");
    fwrite(gff_frame, sizeof(gff_app_frame_t), 1, file);
    fclose(file);
    /*
    printf("%d\n", *(uint32_t*)(buf + 0x58));
    FILE *file = fopen("data.dat", "wb");
    fwrite(buf, amt, 1, file);
    fclose(file);
    */
    /*
    frame->ssi_id = gff_frame->rh.id;
    frame->base_width = gff_frame->frame.width;
    frame->base_height = gff_frame->frame.height;
    frame->event = gff_frame->event_filter;
    */
    //frame->icon_id = ssi_frame->icon_id;
    //frame->base_width = ssi_frame->frame.width;
    //frame->base_height = ssi_frame->frame.height;

    //render_entry_as_image(gff_idx, gff_type, button->icon_id, open_files[pal_idx].pals->palettes, 320, 92);

    // palette is r0
    return EXIT_SUCCESS;
read_error:
no_header:
    return EXIT_FAILURE;
}


#define CONFIG_LEN (1<<15)

extern int soloscuro_generate_window(soloscuro_state_t *state, int window_id) {
    gff_window_t  *win;
    gff_manager_t *man;
    char          *buf;
    char           config[CONFIG_LEN];
    print_args_t   args;
    gff_accl_t     accl;

    man = soloscuro_get_gff_manager();

    memset(&args, 0x0, sizeof(args));
    args.game = DARKSUN_1;
    args.config = config;
    args.len = CONFIG_LEN;
    args.man = man;

    if (gff_read_window(man->ds1.resource, window_id, &win)) {
        goto no_window;
    }

    //gff_chunk_header_t chunk;
    //gff_find_chunk_header(man->ds1.resource, &chunk, GFF_WIND, window_id);
    //gff_read_chunk(man->ds1.resource, &chunk, buf, 4096);
    //win = (gff_window_t*) buf;
    printf("w:%d x h:%d\n", win->frame.width, win->frame.height);
    printf("id: %d\n", win->rh.id);
    printf("offsetx: %d\n", win->offsetx);
    printf("sx: %d, sy: %d\n", win->x, win->y);
    printf("number of items: %d\n", win->itemCount);

    buf = (char*)win;
    args.win = win;
    for (int i = 0; i < win->itemCount; i++) {
        gff_gui_item_t *item = (gff_gui_item_t*)(buf + sizeof(gff_window_t) + 12 + i *(sizeof(gff_gui_item_t)));
        args.item = item;
        //printf("item (0x%x): %d, ", item->type, item->id);
        switch (item->type) {
            case GFF_ACCL:
                load_accl_from_gff(man, item->id);
                break;
            case GFF_APFM:
                printf("APFM");
                load_frame_from_gff(man, item->id);
                /*
                ret->num_frames++;
                ret->frames = realloc(ret->frames, sizeof(sol_frame_t) * ret->num_frames);
                load_frame_from_gff(item->id, ret->frames + (ret->num_frames - 1));
                ret->frames[ret->num_frames - 1].offsetx = item->init_bounds.xmin;
                ret->frames[ret->num_frames - 1].offsety = item->init_bounds.ymin;
                */
                break;
            case GFF_BUTN:
                print_button(&args);
                break;
            case GFF_EBOX:
                print_ebox(&args, item);
                /*
                ret->num_boxes++;
                ret->boxes = realloc(ret->boxes, sizeof(sol_button_t) * ret->num_boxes);
                load_box_from_gff(item->id, ret->boxes + (ret->num_boxes - 1));
                ret->boxes[ret->num_boxes - 1].offsetx = item->init_bounds.xmin;
                ret->boxes[ret->num_boxes - 1].offsety = item->init_bounds.ymin;
                */
                break;
            default:
                printf("UNKNOWN TYPE IN WINDOW: %d\n", item->type);
        }
    }

    int i = 0;
    gff_button_t *b = soloscuro_window_button_extra(window_id, i++);
    while (b) {
        print_button_arg(&args, b, GFF_BMP);
        b = soloscuro_window_button_extra(window_id, i++);
    }

    if (print_window(&args)) {
        goto window_error;
    }

    //apply_overlay(ret);
    printf("config(%d):\n%s\n", strlen(args.config), args.config);
    orxConfig_LoadFromMemory(args.config, args.pos);
    //snprintf(config, CONFIG_LEN, "Window%d", window_id);
    //orxObject_CreateFromConfig(config);
    free(win);

no_window:
window_error:
    return EXIT_FAILURE;
}

static void print_rect(gff_rect_t *r) {
    printf("(%d, %d) -> (%d, %d)",
            r->xmin, r->ymin,
            r->xmax, r->ymax);
}

static int print_window(print_args_t *args) {
    //printf("%d, %d\n", args->win->frame.bounds.xmin, args->win->frame.bounds.ymin);
    //printf("%d, %d\n", args->win->frame.initbounds.xmin, args->win->frame.initbounds.ymin);
    //printf("%d, %d\n", args->win->frame.zonebounds.xmin, args->win->frame.zonebounds.ymin);
    if (append_config(args,
            "[Window%dBackground]\n"
            "Graphic  = Window%dBackgroundTex\n"
            "Position = (%d, %d, 0)\n"
            "\n"
            "[Window%dBackgroundTex]\n"
            "Texture  = ds1.resource.%d.0.r0.bmp\n"
            "\n",
            args->win->rh.id,
            args->win->rh.id,
            soloscuro_window_backgroundx(args->win->rh.id),
            soloscuro_window_backgroundy(args->win->rh.id),
            args->win->rh.id,
            soloscuro_window_background(args->win->rh.id)
            )) {
        goto overflow;
    }

    printf("window: id: %d, bmp: %d/%d, count: %d, offsety:%d\n", args->win->rh.id,
            args->win->frame.background_bmp,
            args->win->frame.border_bmp,
            args->win->itemCount,
            args->win->offsety
            );

    if (append_config(args,
            //"[Window%d@DS1Window]\n"
            "[Window%d@DS1Window]\n"
            "ChildList = ",
            args->win->rh.id)) {
        goto overflow;
    }

    if (args->win->rh.id == 3011) {
        append_config(args, "Window3012 # Window3013 #");
    }

    for (int i = args->button_num - 1; i >= 0; i--) {
        if (append_config(args,
                "Window%dButton%dObject # ",
                args->win->rh.id, i)) {
            goto overflow;
        }
    }

    for (int i = args->ebox_num - 1; i >= 0; i--) {
        if (append_config(args,
                "Window%dBox%d # ",
                args->win->rh.id, i)) {
            goto overflow;
        }
    }

    // Any other children?
    if (append_config(args,
            "Window%dBackground\n"
            "Position = (%d, %d, 0)\n",
            args->win->rh.id,
            args->win->x, args->win->y)) {
        goto overflow;
    }

    return EXIT_SUCCESS;

overflow:
    return EXIT_FAILURE;
}

static int print_button(print_args_t *args) {
    gff_button_t button;
    int gff_type = GFF_ICON;

    if (gff_read_button(args->man->ds1.resource, args->item->id, &button)) {
        goto read_error;
    }

    button.frame.initbounds.xmin = args->item->init_bounds.xmin;
    button.frame.initbounds.ymin = args->item->init_bounds.ymin;

    if (!button.icon_id) {
        switch (args->win->rh.id) {
            case DS1_WINDOW_NEW_CHARACTER:
                switch (args->button_num) {
                    case 0: gff_type = GFF_BMP; button.icon_id = 20084; break;
                    case 9: gff_type = GFF_BMP; button.icon_id = 20085; break;
                    case 10: gff_type = GFF_BMP; button.icon_id = 20083; break;
                }
                break;
        }
    }

    return print_button_arg(args, &button, gff_type);

read_error:
    return EXIT_FAILURE;
}

static int print_button_arg(print_args_t *args, gff_button_t *button, int gff_type) {
    int          frames;
    int          is_anim_button;

    //frames = gff_get_frame_count(args->man->ds1.resource, GFF_ICON, args->item->id);
    //frames = gff_get_frame_count(args->man->ds1.resource, GFF_ICON, button->icon_id);
    frames = gff_get_frame_count(args->man->ds1.resource, gff_type, button->icon_id);
    //printf("OTHER: %d\n", gff_get_frame_count(args->man->ds1.resource, gff_type, button.icon_id));

    is_anim_button = (button->icon_id > 0 && frames >= 2);
    
    if (is_anim_button && append_config(args,
               "[Window%dButton%dAnimSet@DS1ButtonAnimSet%dFrames]\n"
               "Prefix = Window%dButton%d\n"
               "\n",
               args->win->rh.id, args->button_num,
               frames,
               args->win->rh.id, args->button_num)) {
        goto overflow; }
    append_config(args,
           "[Window%dButton%d@DS1Button]\n",
           args->win->rh.id, args->button_num
    );

    if (button->icon_id && append_config(args,
               "%sGraphic  = Window%dButton%dTex0\n",
               (!button->icon_id) ? ";" : "", args->win->rh.id, args->button_num)) {
        goto overflow;
    }

    if (append_config(args,
           "Position = (%d, %d, %d)\n",
           // The real positon is:
           // (args->item->init_bounds.xmin, args->item->init_bounds.ymin, ???)
           // but to support extra buttons, those are copied over to
           // button->frame.initbounds.{xmin, ymin}
           button->frame.initbounds.xmin, button->frame.initbounds.ymin, 0)) {
        goto overflow;
    }

    printf("Pos: %d, %d, bounds: %d %d\n",
           args->item->init_bounds.xmin, args->item->init_bounds.ymin,
           button->iconx, button->frame.initbounds.ymin);

    if (is_anim_button && append_config(args,
               "AnimationSet   = Window%dButton%dAnimSet\n",
               args->win->rh.id, args->button_num)) {
        goto overflow;
    }

    append_config(args, "\n");

    printf("icon:%d, w:%d h:%d, frames: %d\n", button->icon_id, button->frame.width, button->frame.height, frames);
    printf("%d\n", button->frame.background_bmp);
    printf("%d\n", button->frame.border_bmp);
    //if (!button->icon_id) {
        char name[128];
        sprintf(name, "button%d.dat", args->button_num);
        FILE *file = fopen(name, "wb");
        fwrite(button, sizeof(gff_button_t), 1, file);
        fclose(file);
    //}

    for (int i = 0; i < frames; i++) {
        append_config(args,
               "[Window%dButton%dTex%d]\n"
               "Texture  = ds1.resource.%d.%d.r0.%s\n"
               "\n",
               args->win->rh.id, args->button_num, i,
               button->icon_id, i,
               (gff_type == GFF_ICON) ? "icon" : "bmp"
        );
    }

    /*
[Window3500Box0TextText]
String = "Test! THe quick brown fox jumps over the lazy 
dog."
Font = DS1Yellow

[Window3500Box0TextGraphic]
Text  = Window3500Box0TextText
Pivot = top left

[Window3500Box0Text]
Graphic = Window3500Box0TextGraphic
Size     = (239, 10, 0)
Position = (56, 6, 0)
*/
    append_config(args,
           "[Window%dButton%dTextText]\n"
           "String = \n"
           "Font = %s\n\n",
           args->win->rh.id, args->button_num,
           //args->button_num,
           soloscuro_ds1button_font(args->win->rh.id, args->button_num)
    );

    append_config(args,
           "[Window%dButton%dTextGraphic]\n"
           "Text  = Window%dButton%dTextText\n"
           "Pivot = top left\n\n",
           //"Pivot = center\n\n",
           args->win->rh.id, args->button_num,
           args->win->rh.id, args->button_num
    );

    int width = button->frame.width ? button->frame.width : 10;
    int height = button->frame.height ? button->frame.height : 10;
    int sx = button->frame.initbounds.xmin + 3;
    //int sy = button->frame.initbounds.ymin + (height / 2) - 10;
    int sy = button->frame.initbounds.ymin + 1;
    if (height > 20) { sy += (height / 2) - 10; }
    //printf("->>>> %d, %d\n", button->frame.initbounds.xmax, button->frame.initbounds.ymax);

    append_config(args,
           "[Window%dButton%dText@DS1Text]\n"
           "Graphic  = Window%dButton%dTextGraphic\n"
           "Size     = (%d, %d, 0)\n"
           //"Pivot = center\n",
           "Position = (%d, %d, 0)\n\n",
           args->win->rh.id, args->button_num,
           args->win->rh.id, args->button_num,
           width, height,
           sx, sy
    );

    append_config(args,
           "[Window%dButton%dObject@DS1Button]\n"
           //"ChildList = Window%dButton%dText # Window%dButton%d\n\n",
           "ChildList = Window%dButton%dText",// # Window%dButton%d\n\n",
           args->win->rh.id, args->button_num,
           //args->win->rh.id, args->button_num,
           args->win->rh.id, args->button_num
    );
    if (button->icon_id) {
        append_config(args,
               "# Window%dButton%d\n\n",
               args->win->rh.id, args->button_num);
    } else {
        append_config(args, "\n\n");
    }

    args->button_num++;

    return EXIT_SUCCESS;
overflow:
    args->config[args->pos] = '\0';
read_error:
    return EXIT_FAILURE;
}
