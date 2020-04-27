#ifndef GB_TEXTURE_RESOURCE_H_INCLUDED
#define GB_TEXTURE_RESOURCE_H_INCLUDED

#define GB_TEXTURE_MAX_TEXTURES 100
#define GB_TEXTURE_NO_TEXTURE GB_TEXTURE_MAX_TEXTURES + 1

SDL_Texture *gbTextures[GB_TEXTURE_MAX_TEXTURES];

void gbTextureInit();
void gbTextureTeardown();

unsigned int gbTextureLoad(const char *filename);
void gbTextureUnload(unsigned int index);

#endif // TEXTURE_RESOURCE_H_INCLUDED
