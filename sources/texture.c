#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include "../headers/renderer.h"
#include "../headers/texture.h"

static SDL_Texture *g_textures[NUM_TEXTURES];

int loadTexture(char* filename, G_TEXTURE index) {
    SDL_Surface* tempSurface = IMG_Load(filename);

    if (tempSurface == NULL) {
        fprintf(stderr, "Failed to load image from file: %s", IMG_GetError());
        return 0;
    }

    g_textures[index] = SDL_CreateTextureFromSurface(gb_main_renderer, tempSurface);

    if (g_textures[index] == NULL) {
        fprintf(stderr, "Failed to generate texture from surface: %s", IMG_GetError());
        SDL_FreeSurface(tempSurface);
        return 0;
    }

    SDL_FreeSurface(tempSurface);

    return 1;
}

void initTextures() {
    for (uint8_t i = 0; i < NUM_TEXTURES; i++) {
        g_textures[i] = NULL;
    }

    loadTexture("./data/axis_aligned.png", AXIS_ALIGNED_TEXTURE);
    loadTexture("./data/iso.png", ISO_TEXTURE);
}

void destroyTextures() {
    for (int i = 0; i < NUM_TEXTURES; i++) {
        if (g_textures[i] != NULL) {
            SDL_DestroyTexture(g_textures[i]);
            g_textures[i] = NULL;
        }
    }
}

void textureRenderXYClip(G_TEXTURE index, SDL_Rect *dst, SDL_Rect *clip) {
    if (index < 0 || index >= NUM_TEXTURES) {
        fprintf(stderr, "Invalid texture index");
        return;
    }

    SDL_RenderCopyEx(
        gb_main_renderer,
        g_textures[index],
        clip,
        dst,
        0,
        NULL,
        SDL_FLIP_NONE
    );
}
