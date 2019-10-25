#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include "../headers/renderer.h"
#include "../headers/gfx.h"

#define GB_GFX_MAX_SPRITES_PER_LAYER 100

static SDL_Texture *gb_gfx_layer_textures[GFX_LAYER_NUM_LAYERS];

static uint8_t gb_sprite_cursors[GFX_LAYER_NUM_LAYERS];
static GbSprite *gb_gfx_sprites[GFX_LAYER_NUM_LAYERS][GB_GFX_MAX_SPRITES_PER_LAYER];

void gb_gfx_init() {
    for (uint8_t i = 0; i < GFX_LAYER_NUM_LAYERS; i++) {
        gb_gfx_layer_textures[i] = NULL;
    }
}
void gb_gfx_destroy_texture(GB_GFX_LAYER index);
void gb_gfx_teardown() {
    for (int i = 0; i < GFX_LAYER_NUM_LAYERS; i++) {
        gb_gfx_destroy_texture(i);
    }
}

int gb_gfx_load_texture(char* filename, GB_GFX_LAYER index) {
    SDL_Surface* tempSurface = IMG_Load(filename);

    if (tempSurface == NULL) {
        fprintf(stderr, "Failed to load image from file: %s", IMG_GetError());
        return 0;
    }

    gb_gfx_destroy_texture(index);
    gb_gfx_layer_textures[index] = SDL_CreateTextureFromSurface(gb_main_renderer, tempSurface);

    if (gb_gfx_layer_textures[index] == NULL) {
        fprintf(stderr, "Failed to generate texture from surface: %s", IMG_GetError());
        SDL_FreeSurface(tempSurface);
        return 0;
    }

    SDL_FreeSurface(tempSurface);

    return 1;
}

/** PRIVATE FUNCTIONS **/

void gb_gfx_destroy_texture(GB_GFX_LAYER index) {
    if (gb_gfx_layer_textures[index] != NULL) {
        SDL_DestroyTexture(gb_gfx_layer_textures[index]);
        gb_gfx_layer_textures[index] = NULL;
    }
}

//void textureRenderXYClip(G_TEXTURE index, SDL_Rect *dst, SDL_Rect *clip) {
//    if (index < 0 || index >= NUM_TEXTURES) {
//        fprintf(stderr, "Invalid texture index");
//        return;
//    }
//
//    SDL_RenderCopyEx(
//        gb_main_renderer,
//        g_textures[index],
//        clip,
//        dst,
//        0,
//        NULL,
//        SDL_FLIP_NONE
//    );
//}
