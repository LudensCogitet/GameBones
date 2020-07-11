#ifndef SPRITE_TYPE_H_INCLUDED
#define SPRITE_TYPE_H_INCLUDED

#include "SDL2/SDL.h"

#include "SpriteLayer_enum.h"

typedef struct {
    unsigned int id;
    SPRITE_LAYER layer;

    uint8_t fixed;
    uint8_t active;

    unsigned int texture;
    SDL_Rect src;

    uint32_t width;
    uint32_t height;
    SDL_RendererFlip flip;
} Sprite;

#endif // SPRITE_TYPE_H_INCLUDED
