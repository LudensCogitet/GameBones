#include "Sprite_type.h"
#include "SpriteLayer_enum.h"
#include "../gbTexture/gbTexture_sys.h"
#include "../gbRenderer/gbRenderer_sys.h"
#include "../Position_type.h"

#define SPRITE_MAX_PER_LAYER 1000

static unsigned int cursors[SPRITE_MAX_PER_LAYER];
static Position *positions[SPRITE_LAYER_NUM_LAYERS][SPRITE_MAX_PER_LAYER];
static Sprite *sprites[SPRITE_LAYER_NUM_LAYERS][SPRITE_MAX_PER_LAYER];

void spriteInit() {
    for (unsigned int l = 0; l < SPRITE_LAYER_NUM_LAYERS; l++) {
        for (unsigned int s = 0; s < SPRITE_MAX_PER_LAYER; s++) {
            positions[l][s] = 0;
            sprites[l][s] = 0;
        }
    }

    for (unsigned int i = 0; i < SPRITE_LAYER_NUM_LAYERS; i++) {
        cursors[i] = 0;
    }
}

void spriteTeardown() {
    for (unsigned int l = 0; l < SPRITE_LAYER_NUM_LAYERS; l++) {
        for (unsigned int s = 0; s < cursors[l]; s++) {
            positions[l][s] = 0;
            sprites[l][s] = 0;
        }
        cursors[l] = 0;
    }
}

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
                SDL_RendererFlip flip) {
    sprite->texture = texture;

    sprite->src.x = srcX;
    sprite->src.y = srcY;
    sprite->src.w = srcW;
    sprite->src.h = srcH;

    sprite->width    = w;
    sprite->height   = h;

    sprite->layer = layer;
    sprite->active = active;
    sprite->fixed = fixed;
    sprite->flip = flip;
}

int spriteRegister(Sprite *sprite, Position *pos) {
    SPRITE_LAYER layer = sprite->layer;
    if (cursors[layer] >= SPRITE_MAX_PER_LAYER) {
        return 0;
    }

    sprite->id = cursors[layer];

    positions[layer][cursors[layer]] = pos;
    sprites[layer][cursors[layer]] = sprite;

    return cursors[layer]++;
}

void spriteUnregister(Sprite *sprite) {
    SPRITE_LAYER layer = sprite->layer;
    unsigned int index = sprite->id;

    positions[layer][index] = 0;
    sprites[layer][index] = 0;

    if (--cursors[layer] > 0 && cursors[layer] != index) {
        positions[layer][index] = positions[layer][cursors[layer]];
        sprites[layer][index] = sprites[layer][cursors[layer]];
        sprites[layer][index]->id = index;
    }

    positions[layer][cursors[layer]] = 0;
    sprites[layer][cursors[layer]] = 0;
}

void draw(SPRITE_LAYER l) {
    static SDL_Rect dst;
    for (unsigned int s = 0; s < cursors[l]; s++) {
        if (!sprites[l][s]->active) continue;

        dst.x = positions[l][s]->x * gbScaleFactorX;
        dst.y = positions[l][s]->y * gbScaleFactorY;
        dst.w = sprites[l][s]->width * gbScaleFactorX;
        dst.h = sprites[l][s]->height * gbScaleFactorY;

//            if (!sprites[l][s]->fixed) {
//                dst.x -= gbGfxCameraOffsetX * gbScaleFactorX;
//                dst.y -= gbGfxCameraOffsetY * gbScaleFactorY;
//            }

        SDL_RenderCopyEx(
            gbMainRenderer,
            gbTextures[sprites[l][s]->texture],
            &sprites[l][s]->src,
            &dst,
            0,
            0,
            sprites[l][s]->flip
        );
    }
}

void backgroundDraw() {
    draw(SPRITE_LAYER_BACKGROUND);
}

void spriteDraw() {
    for (unsigned int l = 1; l < SPRITE_LAYER_NUM_LAYERS; l++) {
        draw(l);
    }
}
