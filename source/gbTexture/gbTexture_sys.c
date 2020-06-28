#include <stdio.h>
#include "SDL2/SDL.h"
#include <SDL2/SDL_image.h>

#include "../gbResourceManagement/gbResManagement.h"
#include "../gbRenderer/gbRenderer_sys.h"
#include "./gbTexture_sys.h"

static unsigned int cursor;
SDL_Texture *gbTextures[GB_TEXTURE_MAX_TEXTURES];

void gbTextureInit() {
    cursor = 0;

    for (unsigned int i = 0; i < GB_TEXTURE_MAX_TEXTURES; i++) {
        gbTextures[i] = 0;
    }
}

void gbTextureTeardown() {
    for (unsigned int i = 0; i < GB_TEXTURE_MAX_TEXTURES; i++) {
        SDL_DestroyTexture(gbTextures[i]);
        gbTextures[i] = 0;
    }

    cursor = 0;
}

void gbTextureUnload(unsigned int index) {
    if (gbTextures[index] != 0) {
        SDL_DestroyTexture(gbTextures[index]);
    }

    gbTextures[index] = 0;
}

unsigned int gbTextureLoadFromSurface(SDL_Surface *surface) {
    cursor = gbNextEmpty(gbTextures, cursor, GB_TEXTURE_MAX_TEXTURES);
    if (cursor == GB_TEXTURE_MAX_TEXTURES) {
        fprintf(stderr, "Failed to load texture: Texture limit reached");
        return 0;
    }

    gbTextures[cursor] = SDL_CreateTextureFromSurface(gbMainRenderer, surface);

    if (gbTextures[cursor] == 0) {
        fprintf(stderr, "Failed to generate texture from surface: %s", IMG_GetError());
        return 0;
    }

    return cursor;
}

void gbTextureLoadToIndexFromSurface(unsigned int index, SDL_Surface *surface) {
    gbTextureUnload(index);
    gbTextures[index] = SDL_CreateTextureFromSurface(gbMainRenderer, surface);

    if (gbTextures[index] == 0) {
        fprintf(stderr, "Failed to generate texture from surface: %s", IMG_GetError());
        return 0;
    }
}

unsigned int gbTextureLoadFromFile(const char* filename) {
    SDL_Surface *tempSurface = IMG_Load(filename);

    if (tempSurface == 0) {
        fprintf(stderr, "Failed to load image from file: %s", IMG_GetError());
        return 0;
    }

    unsigned int index = gbTextureLoadFromSurface(tempSurface);

    SDL_FreeSurface(tempSurface);

    return index;
}
