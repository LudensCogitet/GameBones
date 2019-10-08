#ifndef TEXTURE_H_INCLUDED
#define TEXTURE_H_INCLUDED

#include <SDL2/SDL.h>

typedef enum G_TEXTURE{
    AXIS_ALIGNED_TEXTURE,
    ISO_TEXTURE,
    NUM_TEXTURES
} G_TEXTURE;

void initTextures();
void destroyTextures();
void textureRenderXYClip(G_TEXTURE index, SDL_Rect *dst, SDL_Rect *clip);

#endif // TEXTURE_H_INCLUDED
