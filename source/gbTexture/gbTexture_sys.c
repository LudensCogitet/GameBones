#include <stdio.h>
#include "SDL2/SDL.h"
#include <SDL2/SDL_image.h>

#include "../gbResourceManagement/gbResManagement.h"
#include "../gbRenderer/gbRenderer_sys.h"
#include "./gbTexture_sys.h"

#include "gbTextureFilePaths.h"

static unsigned int cursor;
SDL_Texture *gbTextures[GB_TEXTURE_MAX_TEXTURES];

void gbTextureInit() {
    cursor = GB_TEXTURE_DYNAMIC_TEXTURE_START;

    for (unsigned int i = 0; i < GB_TEXTURE_MAX_TEXTURES; i++) {
        gbTextures[i] = 0;
    }
}

void gbTextureTeardown() {
    for (unsigned int i = 0; i < GB_TEXTURE_MAX_TEXTURES; i++) {
        if (!gbTextures[i]) continue;

        SDL_DestroyTexture(gbTextures[i]);
        gbTextures[i] = 0;
    }

    cursor = GB_TEXTURE_DYNAMIC_TEXTURE_START;
}

void gbTextureUnload(unsigned int index) {
    if (gbTextures[index] != 0) {
        SDL_DestroyTexture(gbTextures[index]);
    }

    gbTextures[index] = 0;
}

int gbTextureLoadFromSurface(SDL_Surface *surface) {
    cursor = gbNextEmpty((void **)gbTextures, cursor, GB_TEXTURE_MAX_TEXTURES);
    if (cursor == GB_TEXTURE_MAX_TEXTURES) {
        fprintf(stderr, "Failed to load texture: Texture limit reached");
        return -1;
    }

    gbTextures[cursor] = SDL_CreateTextureFromSurface(gbMainRenderer, surface);

    if (gbTextures[cursor] == 0) {
        fprintf(stderr, "Failed to generate texture from surface: %s", IMG_GetError());
        return -1;
    }

    return cursor;
}

int gbTextureLoadToIndexFromSurface(unsigned int index, SDL_Surface *surface) {
    gbTextureUnload(index);
    gbTextures[index] = SDL_CreateTextureFromSurface(gbMainRenderer, surface);

    if (gbTextures[index] == 0) {
        fprintf(stderr, "Failed to generate texture from surface: %s", IMG_GetError());
        return -1;
    }

    return index;
}

int gbTextureLoadFromFile(const char* filename) {
    SDL_Surface *tempSurface = IMG_Load(filename);

    if (tempSurface == 0) {
        fprintf(stderr, "Failed to load image from file: %s", IMG_GetError());
        return -1;
    }

    int index = gbTextureLoadFromSurface(tempSurface);

    SDL_FreeSurface(tempSurface);

    return index;
}

int gbTextureLoadNamed(GB_TEXTURE_NAME name) {
    if (gbTextures[name]) {
        return name;
    }

    SDL_Surface *tempSurface = IMG_Load(gbTextureFilePaths[name]);

    if (tempSurface == 0) {
        fprintf(stderr, "Failed to load image from file: %s", IMG_GetError());
        return -1;
    }

    gbTextures[name] = SDL_CreateTextureFromSurface(gbMainRenderer, tempSurface);

    if (gbTextures[name] == 0) {
        fprintf(stderr, "Failed to generate texture from surface: %s", IMG_GetError());
        return -1;
    }

    SDL_FreeSurface(tempSurface);

    return name;
}
