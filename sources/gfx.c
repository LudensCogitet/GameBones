#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <stdio.h>
#include <stdlib.h>

#include "../headers/renderer.h"
#include "../headers/gfx.h"
#include "../headers/textures.h"

#define GB_GFX_MAX_SPRITES_PER_LAYER 100

static SDL_Texture *gb_gfx_textures[GFX_TEXTURE_NUM_TEXTURES];

static unsigned int gb_gfx_sprite_cursors[GFX_LAYER_NUM_LAYERS];
static GbSprite *gb_gfx_sprites[GFX_LAYER_NUM_LAYERS][GB_GFX_MAX_SPRITES_PER_LAYER];

void gb_gfx_init() {
    for (unsigned int l = 0; l < GFX_LAYER_NUM_LAYERS; l++) {
        for (unsigned int s = 0; s < GB_GFX_MAX_SPRITES_PER_LAYER; s++) {
            gb_gfx_sprites[l][s] = NULL;
        }
    }

    for (unsigned int i = 0; i < GFX_TEXTURE_NUM_TEXTURES; i++) {
        gb_gfx_textures[i] = NULL;
    }

    for (unsigned int i = 0; i < GFX_LAYER_NUM_LAYERS; i++) {
        gb_gfx_sprite_cursors[i] = 0;
    }
}

void gb_gfx_unload_texture(GB_GFX_TEXTURE texture) {
    if (gb_gfx_textures[texture] != NULL) {
        SDL_DestroyTexture(gb_gfx_textures[texture]);
        gb_gfx_textures[texture] = NULL;
    }
}

int gb_gfx_load_texture(char* filename, GB_GFX_TEXTURE texture) {
    SDL_Surface* tempSurface = IMG_Load(filename);

    if (tempSurface == NULL) {
        fprintf(stderr, "Failed to load image from file: %s", IMG_GetError());
        return 0;
    }

    gb_gfx_unload_texture(texture);
    gb_gfx_textures[texture] = SDL_CreateTextureFromSurface(gb_main_renderer, tempSurface);

    if (gb_gfx_textures[texture] == NULL) {
        fprintf(stderr, "Failed to generate texture from surface: %s", IMG_GetError());
        SDL_FreeSurface(tempSurface);
        return 0;
    }

    SDL_FreeSurface(tempSurface);

    return 1;
}

void gb_gfx_teardown() {
    for (int i = 0; i < GFX_TEXTURE_NUM_TEXTURES; i++) {
        gb_gfx_unload_texture(i);
    }
}

void gb_gfx_draw() {
    SDL_RenderClear(gb_main_renderer);

    for (unsigned int l = 0; l < GFX_LAYER_NUM_LAYERS; l++) {
        for (unsigned int s = 0; s < gb_gfx_sprite_cursors[l]; s++) {
            if (gb_gfx_sprites[l][s]->dispose) {
                free(gb_gfx_sprites[l][s]);

                if (--gb_gfx_sprite_cursors[l] > 0) {
                    gb_gfx_sprites[l][s] = gb_gfx_sprites[l][gb_gfx_sprite_cursors[l]];
                }

                gb_gfx_sprites[l][gb_gfx_sprite_cursors[l]] = NULL;

                if (gb_gfx_sprites[l][s] == NULL) break;
            }

            SDL_RenderCopyEx(
                gb_main_renderer,
                gb_gfx_textures[gb_gfx_sprites[l][s]->texture],
                &gb_gfx_sprites[l][s]->src,
                &gb_gfx_sprites[l][s]->dst,
                0,
                NULL,
                SDL_FLIP_NONE
            );
        }
    }

    SDL_RenderPresent(gb_main_renderer);
}

GbSprite *gb_gfx_new_sprite(GB_GFX_LAYER layer, GB_GFX_TEXTURE texture) {
    if (++gb_gfx_sprite_cursors[layer] >= GB_GFX_MAX_SPRITES_PER_LAYER) {
        --gb_gfx_sprite_cursors[layer];
        return NULL;
    }

    GbSprite *newSprite = (GbSprite *)malloc(sizeof(GbSprite));

    newSprite->dispose = 0;

    newSprite->dst.x = 0;
    newSprite->dst.y = 0;
    newSprite->dst.w = 0;
    newSprite->dst.h = 0;

    newSprite->src.x = 0;
    newSprite->src.y = 0;
    newSprite->src.w = 0;
    newSprite->src.h = 0;

    newSprite->texture = texture;

    gb_gfx_sprites[layer][gb_gfx_sprite_cursors[layer] - 1] = newSprite;

    return newSprite;
}
