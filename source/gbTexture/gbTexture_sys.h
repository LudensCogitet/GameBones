#ifndef GB_TEXTURE_RESOURCE_H_INCLUDED
#define GB_TEXTURE_RESOURCE_H_INCLUDED

#include "SDL2/SDL.h"
#include "gbTextureName_enum.h"

#define GB_TEXTURE_MAX_TEXTURES 100
#define GB_TEXTURE_DYNAMIC_TEXTURE_START 50
#define GB_TEXTURE_NO_TEXTURE GB_TEXTURE_MAX_TEXTURES + 1

extern SDL_Texture *gbTextures[GB_TEXTURE_MAX_TEXTURES];

void gbTextureInit();
void gbTextureTeardown();

int gbTextureLoadNamed(GB_TEXTURE_NAME name);
int gbTextureLoadFromFile(const char *filename);
int gbTextureLoadFromSurface(SDL_Surface *surface);
int gbTextureLoadToIndexFromSurface(unsigned int index, SDL_Surface *surface);
void gbTextureUnload(unsigned int index);

#endif // TEXTURE_RESOURCE_H_INCLUDED
