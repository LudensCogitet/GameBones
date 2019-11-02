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
    GB_GFX_TEXTURE texture;
    SDL_Rect src;
    SDL_Rect dst;
} GbSprite;

void gb_gfx_init();
void gb_gfx_teardown();

int gb_gfx_load_texture(char* filename, GB_GFX_TEXTURE texture);
void gb_gfx_unload_texture(GB_GFX_TEXTURE texture);
void gb_gfx_draw();
GbSprite *gb_gfx_new_sprite(GB_GFX_LAYER layer, GB_GFX_TEXTURE texture);

#endif // TEXTURE_H_INCLUDED