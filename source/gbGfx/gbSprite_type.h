#ifndef GB_SPRITE_TYPE_H_INCLUDED
#define GB_SPRITE_TYPE_H_INCLUDED

#include "SDL2/SDL.h"
#include "../gbTexture/gbTexture_sys.h"
#include "../gbEntity/gbPosition_type.h"

typedef struct {
    uint8_t dispose;
    uint8_t fixed;

    unsigned int texture;
    SDL_Rect src;

    gbPosition *pos;
    uint32_t width;
    uint32_t height;
} gbSprite;

#endif // GB_SPRITE_TYPE_H_INCLUDED
