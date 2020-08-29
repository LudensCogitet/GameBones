#ifndef SPRITE_SYS_H_INCLUDED
#define SPRITE_SYS_H_INCLUDED

#include "SDL2/SDL.h"
#include "../gbTexture/gbTexture_sys.h"
#include "SpriteLayer_enum.h"
#include "../Position_type.h"

void spriteInit();
void spriteTeardown();

void spriteSet(Sprite *sprite,
                unsigned int texture,

                int srcX,
                int srcY,
                int srcW,
                int srcH,

                int w,
                int h,
                SPRITE_LAYER layer,
                uint8_t active,
                uint8_t fixed,
                SDL_RendererFlip flip);

void spriteRegister(Sprite *sprite, Position *pos);
void spriteUnregister(Sprite *sprite);
void backgroundDraw();
void spriteDraw();

#endif // SPRITE_SYS_H_INCLUDED
