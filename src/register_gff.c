#include "orx.h"
#include "orxResource.h"

#include <gff/gff.h>
#include <gff/gfftypes.h>
#include <gff/image.h>
#include <gff/manager.h>

#include <ctype.h>
#include <stdint.h>

static const orxSTRING orxFASTCALL soloscuro_gff_locate(const orxSTRING _zStorage, const orxSTRING _zName, orxBOOL _bRequireExistence);
static orxS64 orxFASTCALL          soloscuro_gff_gettime(const orxSTRING _zLocation);
static orxHANDLE orxFASTCALL       soloscuro_gff_open(const orxSTRING _zLocation, orxBOOL _bEraseMode);
static void orxFASTCALL            soloscuro_gff_close(orxHANDLE _hResource);
static orxS64 orxFASTCALL          soloscuro_gff_getsize(orxHANDLE _hResource);
static orxS64 orxFASTCALL          soloscuro_gff_seek(orxHANDLE _hResource, orxS64 _s64Offset, orxSEEK_OFFSET_WHENCE _eWhence);
static orxS64 orxFASTCALL          soloscuro_gff_tell(orxHANDLE _hResource);
static orxS64 orxFASTCALL          soloscuro_gff_read(orxHANDLE _hResource, orxS64 _s64Size, void *_pBuffer);
static orxS64 orxFASTCALL          soloscuro_gff_write(orxHANDLE _hResource, orxS64 _s64Size, const void *_pBuffer);

// Handle for all the gff storage.
static gff_manager_t *gff_manager = NULL;

// Stores all the information for loading from a GFF entry.
typedef struct gff_entry_params_s {
    gff_game_type_t game;
    gff_file_t *f;
    uint32_t type;
    uint32_t res_id;
    uint32_t frame;
    gff_palette_t *pal;
} gff_entry_params_t;

// Represents the gff data in memory.
typedef struct gff_entry_data_s {
    int32_t pos, size;
    char data[];
} gff_entry_data_t;

// Header for loader graphic data.
unsigned char bmp_header[] = {// All values are little-endian
    0x42, 0x4D,             // Signature 'BM'
    0xaa, 0x00, 0x00, 0x00, // Size: 170 bytes
    0x00, 0x00,             // Unused
    0x00, 0x00,             // Unused
    0x8a, 0x00, 0x00, 0x00, // Offset to image data

    0x7c, 0x00, 0x00, 0x00, // DIB header size (124 bytes)
    0x04, 0x00, 0x00, 0x00, // Width (4px)
    0x02, 0x00, 0x00, 0x00, // Height (2px)
    0x01, 0x00,             // Planes (1)
    0x20, 0x00,             // Bits per pixel (32)
    0x03, 0x00, 0x00, 0x00, // Format (bitfield = use bitfields | no compression)
    0x20, 0x00, 0x00, 0x00, // Image raw size (32 bytes)
    0x13, 0x0B, 0x00, 0x00, // Horizontal print resolution (2835 = 72dpi * 39.3701)
    0x13, 0x0B, 0x00, 0x00, // Vertical print resolution (2835 = 72dpi * 39.3701)
    0x00, 0x00, 0x00, 0x00, // Colors in palette (none)
    0x00, 0x00, 0x00, 0x00, // Important colors (0 = all)
    0xFF, 0x00, 0x00, 0x00, // B bitmask (000000FF)
    0x00, 0xFF, 0x00, 0x00, // G bitmask (0000FF00)
    0x00, 0x00, 0xFF, 0x00, // R bitmask (00FF0000)
    0x00, 0x00, 0x00, 0xFF, // A bitmask (FF000000)
    0x42, 0x47, 0x52, 0x73, // sRGB color space
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Unused R, G, B entries for color space
    0x00, 0x00, 0x00, 0x00, // Unused Gamma X entry for color space
    0x00, 0x00, 0x00, 0x00, // Unused Gamma Y entry for color space
    0x00, 0x00, 0x00, 0x00, // Unused Gamma Z entry for color space

    0x00, 0x00, 0x00, 0x00, // Unknown
    0x00, 0x00, 0x00, 0x00, // Unknown
    0x00, 0x00, 0x00, 0x00, // Unknown
    0x00, 0x00, 0x00, 0x00, // Unknown
};

extern int soloscuro_register_gff_type() {
    int status = EXIT_FAILURE;
    if (gff_manager == NULL) {
        gff_manager = gff_manager_create();
        gff_manager_load_ds1(gff_manager, "ds1/");
    }

    orxSTATUS eResult = orxSTATUS_FAILURE;
    orxRESOURCE_TYPE_INFO stTypeInfo;

    /* Inits flags */
    //sstResource.u32Flags = orxRESOURCE_KU32_STATIC_FLAG_READY;

    /* Inits file type */
    stTypeInfo.zTag       = "<gff>";
    stTypeInfo.pfnLocate  = soloscuro_gff_locate;
    stTypeInfo.pfnGetTime = soloscuro_gff_gettime;
    stTypeInfo.pfnOpen    = soloscuro_gff_open;
    stTypeInfo.pfnClose   = soloscuro_gff_close;
    stTypeInfo.pfnGetSize = soloscuro_gff_getsize;
    stTypeInfo.pfnSeek    = soloscuro_gff_seek;
    stTypeInfo.pfnTell    = soloscuro_gff_tell;
    stTypeInfo.pfnRead    = soloscuro_gff_read;
    stTypeInfo.pfnWrite   = soloscuro_gff_write;
    //stTypeInfo.pfnDelete  = soloscuro_gff_delete;

    eResult = orxResource_RegisterType(&stTypeInfo);
    if (eResult != orxSTATUS_FAILURE) {
        status = EXIT_SUCCESS;
        goto out;
    }
    
out:
    return EXIT_FAILURE;
}

static int is_gff_name(const orxSTRING name) {
    return (name[0] == 'g'
            && name[1] == 'f'
            && name[2] == 'f');
}

static int get_next_token(const char *ptr, char *buf) {
    int32_t pos = 0;

    while (*ptr != ':' && *ptr != '\0') {
        buf[pos++] = tolower(*ptr);
        ptr++;
    }
    buf[pos] = '\0';

    return *ptr == '\0' ? pos : pos + 1;
}

static int set_game_type_from_string(gff_entry_params_t *params, const char *str) {
    if (!strncmp(str, "ds1", 3)) {
        params->game = DARKSUN_1;
        goto success;
    }

    if (!strncmp(str, "ds2", 3)) {
        params->game = DARKSUN_2;
        goto success;
    }

    if (!strncmp(str, "dso", 3)) {
        params->game = DARKSUN_ONLINE;
        goto success;
    }

    goto unknown_error;
    
success:
    return EXIT_SUCCESS;
    
unknown_error:
    params->game = DARKSUN_UNKNOWN;
    return EXIT_FAILURE;

}

static int set_file_from_ds1_string(gff_entry_params_t *params, const char *str) {
    int region_id;

    if (!strncmp(str, "resource", 8)) {
        params->f = gff_manager->ds1.resource;
        goto success;
    }

    if (!strncmp(str, "segobjex", 8)) {
        params->f = gff_manager->ds1.segobjex;
        goto success;
    }

    if (!strncmp(str, "gpl", 3)) {
        params->f = gff_manager->ds1.gpl;
        goto success;
    }

    if (!strncmp(str, "cine", 3)) {
        params->f = gff_manager->ds1.cine;
        goto success;
    }
    
    if (sscanf(str, "rgn%d", &region_id) == 1) {
        if (region_id == 255) {
            params->f = gff_manager->ds1.wild_region;
            goto success;
        }
        if (region_id >= 0 && region_id < DS1_MAX_REGIONS) {
            params->f = gff_manager->ds1.regions[region_id];
            goto success;
        }
    }

    goto unknown_type_error;

success:
    return EXIT_SUCCESS;

unknown_type_error:
    return EXIT_FAILURE;
}

static int set_file_from_string(gff_entry_params_t *params, const char *str) {
    switch (params->game) {
        case DARKSUN_1 : return set_file_from_ds1_string(params, str);
    }

    return EXIT_FAILURE;
}

static int set_entry_type_from_string(gff_entry_params_t *params, const char *str) {
    if (!strncmp("icon", str, 4)) {
        params->type = GFF_ICON;
        goto success;
    }

    if (!strncmp("tile", str, 4)) {
        params->type = GFF_TILE;
        goto success;
    }

    if (!strncmp("bmp", str, 3)) {
        params->type = GFF_BMP;
        goto success;
    }

    if (!strncmp("port", str, 4)) {
        params->type = GFF_PORT;
        goto success;
    }

    if (!strncmp("wall", str, 4)) {
        params->type = GFF_WALL;
        goto success;
    }
    
    goto unknown_type;

success:
    return EXIT_SUCCESS;

unknown_type:
    return EXIT_FAILURE;
}

static int set_palette_from_ds1_string(gff_entry_params_t *params, const char *str) {
    int palette_num = 0;

    if (str[0] == 'c') {
        if (str[1] != '\0') {
            sscanf(str, "c%d", &palette_num);
        }
        params->pal = params->f->pals->palettes + palette_num;
        goto success;
    }

    if (sscanf(str, "r%d", &palette_num)) {
        params->pal = gff_manager->ds1.resource->pals->palettes + palette_num;
        goto success;
    }

    goto dne;
success:
    return EXIT_SUCCESS;
dne:
    return EXIT_FAILURE;
}

static int set_palette_from_string(gff_entry_params_t *params, const char *str) {
    switch(params->game) {
        case DARKSUN_1: return set_palette_from_ds1_string(params, str);
    }

    return EXIT_FAILURE;
}

static int grab_params(const orxSTRING name, gff_entry_params_t *params) {
    const char *ptr = name;
    char buf[128];
    size_t pos = 0;
    char *end_ptr = NULL;

    pos += get_next_token(ptr, buf); // game type
    if (set_game_type_from_string(params, buf)) {
        goto game_type_error;
    }

    pos += get_next_token(ptr + pos, buf); // gff file
    if (set_file_from_string(params, buf)) {
        goto set_file_error;
    }

    pos += get_next_token(ptr + pos, buf);
    if (set_entry_type_from_string(params, buf)) {
        goto entry_type_error;
    }

    pos += get_next_token(ptr + pos, buf);
    params->res_id = strtol(buf, &end_ptr, 10);
    if (buf[0] == '\0' || *end_ptr != '\0') {
        goto res_id_error;
    }

    pos += get_next_token(ptr + pos, buf);
    params->frame = strtol(buf, &end_ptr, 10);
    if (buf[0] == '\0' || *end_ptr != '\0') {
        goto frame_error;
    }

    pos += get_next_token(ptr + pos, buf);
    if (set_palette_from_string(params, buf)) {
        goto palette_error;
    }

    return EXIT_SUCCESS;

palette_error:
frame_error:
res_id_error:
entry_type_error:
set_file_error:
game_type_error:
token_error:
    return EXIT_FAILURE;
}

static int entry_exists(gff_entry_params_t *params) {
    gff_chunk_header_t chunk;
    return gff_find_chunk_header(params->f, &chunk, params->type, params->res_id);
}

static int open_gff_graphic(gff_entry_params_t *params, gff_entry_data_t **data) {
    uint32_t          *bmp_ptr = (uint32_t*)(bmp_header + 2);
    gff_image_entry_t *cimg = NULL;
    gff_entry_data_t  *img = NULL;
    uint32_t          *udata;
    gff_frame_info_t   info;
    gff_chunk_header_t chunk;
    char               type[5];

    if (params->type != GFF_BMP && params->type != GFF_ICON && params->type != GFF_PORT
            && params->type != GFF_TILE && params->type != GFF_WALL) {
        goto not_a_graphic_entry;
    }

    gff_frame_info(params->f, params->type, params->res_id, params->frame, &info);
    //printf("%d x %d\n", info.w, info.h);

    type[4] = '\0';
    type[3] = (params->type >> 24) & 0xFF;
    type[2] = (params->type >> 16) & 0xFF;
    type[1] = (params->type >> 8) & 0xFF;
    type[0] = (params->type >> 0) & 0xFF;

    if (gff_find_chunk_header(params->f, &chunk, params->type, params->res_id)) {
        goto could_not_find_header;
    }

    cimg = (gff_image_entry_t*) malloc(sizeof(gff_image_entry_t) + chunk.length);
    if (gff_read_chunk(params->f, &chunk, &(cimg->data), chunk.length) != chunk.length) {
        goto could_not_read;
    }

    cimg->data_len = chunk.length;
    cimg->frame_num = *(uint16_t*)(cimg->data + 4);

    //printf("%s:%s:%d: has %d frames\n", "resource", type, params->res_id, cimg->frame_num);

    udata = (uint32_t*)gff_get_frame_rgba_palette_img(cimg, 0, params->pal);
    if (!udata) {
        goto rgba_error;
    }
    bmp_ptr[0] = sizeof(bmp_header) + 32 * info.w * info.h; // size
    bmp_ptr[4] = info.w;
    bmp_ptr[5] = info.h;
    img = malloc(sizeof(gff_entry_data_t) + bmp_ptr[0]);
    if (!img) {
        goto img_mem_error;
    }
    memset(img, 0x0, sizeof(gff_entry_data_t));
    img->size = sizeof(bmp_header) + 32 * info.w * info.h; // size
    memcpy(img->data, bmp_header, sizeof(bmp_header));
    memcpy(img->data + sizeof(bmp_header), udata, img->size - sizeof(bmp_header));

    free(udata);
    free(cimg);

    *data = img;
    return EXIT_SUCCESS;

img_mem_error:
    free(udata);
    udata = NULL;
rgba_error:
could_not_read:
    free(cimg);
    cimg = NULL;
could_not_find_header:
not_a_graphic_entry:
    return EXIT_FAILURE;
}

static int open_gff_entry(gff_entry_params_t *params, gff_entry_data_t **data) {
    int status = open_gff_graphic(params, data);
    if (status == EXIT_SUCCESS) {
        goto out;
    }

out:
    return status;
}

static const orxSTRING orxFASTCALL soloscuro_gff_locate(const orxSTRING storage, const orxSTRING name, orxBOOL required) {
    gff_entry_params_t params;

    if (grab_params(name, &params) == EXIT_FAILURE) {
        goto not_a_gff_entry;
    }

    if (entry_exists(&params)) {
        goto entry_dne;
    }

    //printf("storage: %s, name: %s, exists!\n", storage, name);
    return name;

entry_dne:
    //printf("%s Does NOT exist!\n", name);
not_a_gff_entry:
    return orxNULL;
}

static orxS64 orxFASTCALL soloscuro_gff_gettime(const orxSTRING _zLocation) {
    return 0;
}

static orxHANDLE orxFASTCALL soloscuro_gff_open(const orxSTRING location, orxBOOL erase_mode) {
    gff_entry_data_t *data;
    gff_entry_params_t params;
    //printf("open: '%s'\n", location);

    if (grab_params(location, &params) == EXIT_FAILURE) {
        goto not_a_gff_entry;
    }

    if (open_gff_entry(&params, &data)) {
        goto could_not_open;
    }

    return data;
could_not_open:
not_a_gff_entry:
    return NULL;
}

static void orxFASTCALL soloscuro_gff_close(orxHANDLE entry) {
    //printf("close\n");
    free(entry);
    entry = NULL;
}

static orxS64 orxFASTCALL soloscuro_gff_getsize(orxHANDLE res) {
    gff_entry_data_t *data = (gff_entry_data_t*) res;
    //printf("size: %d\n", img->size);
    return data->size;
}

static orxS64 orxFASTCALL soloscuro_gff_seek(orxHANDLE res, orxS64 offset, orxSEEK_OFFSET_WHENCE whence) {
    //printf("seek: offset: %d, whence: %d\n", offset, whence);
    gff_entry_data_t *data = (gff_entry_data_t*) res;

    switch (whence) {
        case orxSEEK_OFFSET_WHENCE_START:
            data->pos = offset;
            if (data->pos > data->size) { data->pos = data->size; }
            break;
        case orxSEEK_OFFSET_WHENCE_CURRENT:
            data->pos += offset;
            if (data->pos > data->size) { data->pos = data->size; }
            if (data->pos < 0)          { data->pos = 0; }
            break;
        case orxSEEK_OFFSET_WHENCE_END:
            data->pos = data->size - offset;
            if (data->pos < 0) { data->pos = 0; }
            break;
        default:
            printf("UNKNOWN WHENCE!\n");
            break;
    }

    return data->pos;
}

static orxS64 orxFASTCALL soloscuro_gff_tell(orxHANDLE res) {
    gff_entry_data_t *data = (gff_entry_data_t*) res;
    return data->pos;
}

static orxS64 orxFASTCALL soloscuro_gff_read(orxHANDLE res, orxS64 len, void *buf) {
    gff_entry_data_t *data = (gff_entry_data_t*) res;
    int amt = (data->size - data->pos)< len
        ? (data->size - data->pos)
        : len;
    if (amt < 0) { amt = 0; }

    //printf("read: len = %d, data_pos = %d, amt = %d\n", len, data_pos, amt);

    memcpy(buf, data->data + data->pos, amt);

    data->pos += amt;

    return amt;
}

static orxS64 orxFASTCALL soloscuro_gff_write(orxHANDLE _hResource, orxS64 _s64Size, const void *_pBuffer) {
    //printf("write\n");
    return 0;
}
