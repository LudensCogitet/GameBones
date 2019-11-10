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

typedef struct GbSprite {
    uint8_t dispose;
    uint8_t fixed;
    GB_GFX_TEXTURE texture;
    SDL_Rect src;
    SDL_Rect dst;
} GbSprite;

void gb_gfx_init();
void gb_gfx_teardown();

int gb_gfx_load_texture(char* filename, GB_GFX_TEXTURE texture);
void gb_gfx_unload_texture(GB_GFX_TEXTURE texture);
void gb_gfx_draw();
GbSprite *gb_gfx_new_sprite(GB_GFX_LAYER layer, GB_GFX_TEXTURE texture, uint8_t fixed);

GbSprite *gb_gfx_text_draw(char *text, uint8_t wrapW, uint8_t fixed);

void gb_gfx_camera_set(int32_t x, int32_t y);
void gb_gfx_camera_move(int32_t dx, int32_t dy);
void gb_gfx_camera_get_pos(int32_t *x, int32_t *y);

#endif // TEXTURE_H_INCLUDED
