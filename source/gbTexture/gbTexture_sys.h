#ifndef GB_TEXTURE_RESOURCE_H_INCLUDED
#define GB_TEXTURE_RESOURCE_H_INCLUDED

#include "SDL2/SDL.h"

#define GB_TEXTURE_MAX_TEXTURES 100
#define GB_TEXTURE_NO_TEXTURE GB_TEXTURE_MAX_TEXTURES + 1

extern SDL_Texture *gbTextures[GB_TEXTURE_MAX_TEXTURES];

void gbTextureInit();
void gbTextureTeardown();

unsigned int gbTextureLoadFromFile(const char *filename);
unsigned int gbTextureLoadFromSurface(SDL_Surface *surface);
void gbTextureLoadToIndexFromSurface(unsigned int index, SDL_Surface *surface);
void gbTextureUnload(unsigned int index);

#endif // TEXTURE_RESOURCE_H_INCLUDED
