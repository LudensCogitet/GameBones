#ifndef GB_SPRITE_TYPE_H_INCLUDED
#define GB_SPRITE_TYPE_H_INCLUDED

#include "SDL2/SDL.h"
#include "../../gbTexture/gbTexture_sys.h"
#include "../../gbEntity/gbPosition_type.h"
#include "./gbSpriteLayer_enum.h"

typedef struct {
    uint8_t dispose;
    uint8_t fixed;
    uint8_t active;

    unsigned int texture;
    SDL_Rect src;

    gbPosition *pos;
    uint32_t width;
    uint32_t height;
    SDL_RendererFlip flip;
} gbSprite;

gbSprite *gbSpriteNew(
    GB_SPRITE_LAYER layer,
    unsigned int texture,

    int srcX,
    int srcY,
    int srcW,
    int srcH,

    gbPosition *pos,
    int w,
    int h,
    uint8_t active,
    uint8_t fixed,
    SDL_RendererFlip flip
);

void gbSpriteInit();
void gbSpriteTeardown();
void gbSpriteDrawSprites();
#endif // GB_SPRITE_TYPE_H_INCLUDED
