#ifndef TEXTURE_H_INCLUDED
#define TEXTURE_H_INCLUDED

#include <SDL2/SDL.h>
#include <stdint.h>
#include "./textures.h"

typedef enum GB_GFX_LAYER {
    GFX_LAYER_BACKGROUND,
    GFX_LAYER_MIDGROUND,
    GFX_LAYER_FOREGROUND,
    GFX_LAYER_NUM_LAYERS
} GB_GFX_LAYER;

typedef enum GB_GFX_FONT {
    GFX_FONT_LARGE_FREE_MONO,
    GFX_FONT_NUM_FONTS
} GB_GFX_FONT;

typedef enum GB_GFX_COLOR {
    GFX_COLOR_WHITE,
    GFX_COLOR_BLACK,
    GFX_COLOR_NUM_COLORS
} GB_GFX_COLOR;

typedef struct GbSprite {
    uint8_t dispose;
    uint8_t fixed;
    GB_GFX_TEXTURE texture;
    SDL_Rect src;
    SDL_Rect dst;
} GbSprite;

void gb_gfx_init();
void gb_gfx_teardown();

int gb_gfx_texture_load(char* filename, GB_GFX_TEXTURE texture);
void gb_gfx_texture_unload(GB_GFX_TEXTURE texture);

void gb_gfx_sprite_move(double x, double y, GbSprite *sprite);
void gb_gfx_draw();

GbSprite *gb_gfx_new_sprite(
    GB_GFX_LAYER layer,
    GB_GFX_TEXTURE texture,
    int srcX,
    int srcY,
    int srcW,
    int srcH,
    double x,
    double y,
    int w,
    int h,
    uint8_t fixed
);

void gb_gfx_font_load(char *ttfFile, GB_GFX_FONT font, uint16_t pt);
void gb_gfx_font_unload(GB_GFX_FONT font);

void gb_gfx_font_set(GB_GFX_FONT font);
void gb_gfx_font_color_set(GB_GFX_COLOR color);
void gb_gfx_font_layer_set(GB_GFX_LAYER layer);

GbSprite *gb_gfx_new_text(char *text, uint32_t wrapW, double x, double y, uint8_t fixed);
void gb_gfx_text_change(GbSprite *sprite, char *text, uint32_t wrapW);

void gb_gfx_camera_set(int32_t x, int32_t y);
void gb_gfx_camera_move(int32_t dx, int32_t dy);
void gb_gfx_camera_get_pos(int32_t *x, int32_t *y);
void gb_gfx_camera_follow(double *x, double *y, uint32_t distance_x, uint32_t distance_y);
void gb_gfx_camera_unfollow();

#endif // TEXTURE_H_INCLUDED
