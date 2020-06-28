#include "./gbSprite_type.h"
#include "./gbSpriteLayer_enum.h"
#include "../../gbRenderer/gbRenderer_sys.h"
#define GB_SPRITE_MAX_PER_LAYER 1000

static unsigned int cursors[GB_SPRITE_MAX_PER_LAYER];
static gbSprite     *sprites[GB_SPRITE_MAX_PER_LAYER][GB_SPRITE_MAX_PER_LAYER];

void gbSpriteInit() {
    for (unsigned int l = 0; l < GB_SPRITE_LAYER_NUM_LAYERS; l++) {
        for (unsigned int s = 0; s < GB_SPRITE_MAX_PER_LAYER; s++) {
            sprites[l][s] = 0;
        }
    }

    for (unsigned int i = 0; i < GB_SPRITE_LAYER_NUM_LAYERS; i++) {
        cursors[i] = 0;
    }
}

void gbSpriteTeardown() {
    for (unsigned int l = 0; l < GB_SPRITE_LAYER_NUM_LAYERS; l++) {
        for (unsigned int s = 0; s < cursors[l]; s++) {
            free(sprites[l][s]);
            sprites[l][s] = 0;
        }
        cursors[l] = 0;
    }
}

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
) {
    if (cursors[layer] + 1 >= GB_SPRITE_MAX_PER_LAYER) {
        return 0;
    }

    gbSprite *newSprite = (gbSprite *)malloc(sizeof(gbSprite));

    newSprite->dispose = 0;
    newSprite->active = active;

    newSprite->width    = w;
    newSprite->height   = h;

    newSprite->pos = pos;

    newSprite->src.x = srcX;
    newSprite->src.y = srcY;
    newSprite->src.w = srcW;
    newSprite->src.h = srcH;

    newSprite->texture = texture;
    newSprite->flip = flip;

    sprites[layer][cursors[layer]++] = newSprite;

    return newSprite;
}

void gbSpriteDrawSprites() {
    static SDL_Rect dst;
    for (unsigned int l = 0; l < GB_SPRITE_MAX_PER_LAYER; l++) {
        for (unsigned int s = 0; s < cursors[l]; s++) {
            if (sprites[l][s]->dispose) {
                free(sprites[l][s]);

                if (--cursors[l] > 0) {
                    sprites[l][s] = sprites[l][cursors[l]];
                }

                sprites[l][cursors[l]] = 0;

                if (sprites[l][s] == 0) break;
            }

            if (!sprites[l][s]->active) continue;

            dst.x = sprites[l][s]->pos->x * gbScaleFactorX;
            dst.y = sprites[l][s]->pos->y * gbScaleFactorY;
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
}
