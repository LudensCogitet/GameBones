#ifndef TEXTURE_H_INCLUDED
#define TEXTURE_H_INCLUDED

#include <SDL2/SDL.h>

typedef enum GB_GFX_LAYER {
    GFX_LAYER_BACKGROUND,
    GFX_LAYER_MIDGROUND,
    GFX_LAYER_FOREGROUND,
    GFX_LAYER_NUM_LAYERS
} GB_GFX_LAYER;

typedef struct GbSprite {
    SDL_Rect src;
    SDL_Rect dst;
} GbSprite;

void gb_gfx_init();
void gb_gfx_teardown();

int gb_gfx_load_texture(char* filename, GB_GFX_LAYER index);

//void textureRenderXYClip(G_TEXTURE index, SDL_Rect *dst, SDL_Rect *clip);

#endif // TEXTURE_H_INCLUDED
