#include <stdlib.h>
#include "./entityDino.h"
#include "../../gbGfx/gbGfx_sys.h"
#include "../../gbEntity/gbPosition_type.h"
#include "../../gbInput/gbInput_sys.h"
#include "../../gbInput/gbInputState_type.h"
#include "../../gbAnimation/gbAnimation_sys.h"
#include "../../gbAnimation/gbAnimation_type.h"
#include "../../gbAnimation/gbAnimType_type.h"

static unsigned int dinoTexture = GB_TEXTURE_NO_TEXTURE;
static unsigned int dinoAnimationIdle = GB_ANIMATION_NO_ANIMATION;

gbEntity *dinoNew(double x, double y) {
    if (dinoTexture == GB_TEXTURE_NO_TEXTURE) {
        dinoTexture = gbTextureLoad("./assets/dino.png");
        dinoAnimationIdle = gbAnimationNew(0, 0, 24, 0, 4, 3, 1, GB_ANIM_TYPE_LOOP);

        gbEntityThink[GB_ENTITY_TYPE_DINO] = (GB_ENTITY_THINK_FUNC)&dinoThink;
        gbEntityRespond[GB_ENTITY_TYPE_DINO] = (GB_ENTITY_RESPOND_FUNC)&dinoRespond;
        gbEntityDispose[GB_ENTITY_TYPE_DINO] = (GB_ENTITY_DISPOSE_FUNC)&dinoDispose;
    }

    Dino *dino = (Dino *)malloc(sizeof(Dino));
    dino->pos = (gbPosition){x, y};

    gbSprite *sprite = gbGfxSpriteNew(
                                      GB_GFX_LAYER_MIDGROUND,
                                      dinoTexture,
                                      0, 0, 24, 24,
                                      &dino->pos, 24, 24,
                                      0);
    dino->sprite = sprite;

    dino->animState = gbAnimationStateNew(dinoAnimationIdle);

    dino->dx = 0;
    dino->dy = 0;

    dino->entity = gbEntityNew(GB_ENTITY_TYPE_DINO, dino, GB_ENTITY_PRIORITY_HIGH);
    return dino->entity;
}

void dinoThink(Dino *dino, double delta) {
        gbAnimationApply(&dino->sprite->src, delta, &dino->animState, dinoAnimationIdle);

        if (gbInputCheckState(GB_INPUT_MOVE_LEFT, GB_INPUT_PRESSED)) {
            dino->dx -= 10 * delta;
        }

        if (gbInputCheckState(GB_INPUT_MOVE_RIGHT, GB_INPUT_PRESSED)) {
            dino->dx += 10 * delta;
        }

        if (gbInputCheckState(GB_INPUT_DESTROY_DINOS, GB_INPUT_RELEASED)) {
            dino->entity->dispose = 1;
        }

        dino->pos.x += dino->dx * delta;
}

void dinoRespond(Dino *dino, gbMessage *messages, uint16_t numMessages) {

}

void dinoDispose(Dino * dino) {
    dino->sprite->dispose = 1;
}
