#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "./entityDino.h"
#include "../../gbRenderer/gbRenderer_sys.h"
#include "../../gbGfx/gbGfx_sys.h"
#include "../../gbEntity/gbPosition_type.h"
#include "../../gbInput/gbInput_sys.h"
#include "../../gbInput/gbInputState_type.h"
#include "../../gbAnimation/gbAnimation_sys.h"
#include "../../gbAnimation/gbAnimation_type.h"
#include "../../gbAnimation/gbAnimType_type.h"

static unsigned int dinoTexture = GB_TEXTURE_NO_TEXTURE;
static unsigned int dinoAnimationIdle = GB_ANIMATION_NO_ANIMATION;

static unsigned int dinoAnimations[DINO_STATE_NUM_STATES] = {
    GB_ANIMATION_NO_ANIMATION
};

static unsigned int dinoCount = 0;

// CONSTS
static const double walkAcceleration = GB_LOGICAL_SCREEN_WIDTH / 2;
static const double stopAcceleration = GB_LOGICAL_SCREEN_WIDTH;
static const double maxVelocity = GB_LOGICAL_SCREEN_WIDTH / 2;
static const double runPoint = 0.75;

// Forward declarations
static void initDino();

static void idle(Dino *dino);
static void speedUp(Dino *dino);
static void slowDown(Dino *dino);
static void run(Dino *dino);
static void handleMoveKeyDown(Dino *dino);
static void handleMoveKeyUp(Dino *dino);

gbEntity *dinoNew(double x, double y, SDL_RendererFlip flip) {
    if (dinoCount++ == 0) {
        initDino();
    }

    printf("Dino count: %d\n", dinoCount);

    Dino *dino = (Dino *)malloc(sizeof(Dino));
    dino->pos = (gbPosition){x, y};

    gbSprite *sprite = gbGfxSpriteNew(
                                      GB_GFX_LAYER_MIDGROUND,
                                      dinoTexture,
                                      0, 0, 24, 24,
                                      &dino->pos, 24, 24,
                                      0,
                                      flip);
    dino->sprite = sprite;

    dino->velocity = 0;
    dino->acceleration = 0;
    dino->direction = 0;

    dino->state = DINO_STATE_IDLE;
    gbAnimationStateInit(dinoAnimations[DINO_STATE_IDLE], &sprite->src, &dino->animState);

    dino->entity = gbEntityNew(GB_ENTITY_TYPE_DINO, dino, GB_ENTITY_PRIORITY_HIGH);
    return dino->entity;
}

void dinoThink(Dino *dino, double delta) {
    switch (dino->state) {
        case DINO_STATE_IDLE:
            idle(dino);
            break;
        case DINO_STATE_SLOW_DOWN:
            slowDown(dino);
            break;
        case DINO_STATE_SPEED_UP:
            speedUp(dino);
            break;
        case DINO_STATE_RUN:
            run(dino);
            break;
    }

    double oldVelocity = dino->velocity;
    dino->velocity += (dino->acceleration * delta);

    if (dino->velocity > maxVelocity) {
        dino->velocity = maxVelocity;
    } else if (dino->velocity < -maxVelocity) {
        dino->velocity = -maxVelocity;
    }

    int oldSign = (int)(oldVelocity > 0) - (oldVelocity < 0);
    int newSign = (int)(dino->velocity > 0) - (dino->velocity < 0);

    oldSign = !oldSign ? 1 : oldSign;
    newSign = !newSign ? 1 : newSign;

    if (oldVelocity != 0 && dino->velocity != 0 && oldSign + newSign == 0) {
        dino->velocity = 0;
        dino->acceleration = 0;
    }

    dino->pos.x += dino->velocity * delta;
    gbAnimationApply(&dino->sprite->src, delta, &dino->animState, dinoAnimations[dino->state]);
}

void dinoRespond(Dino *dino, gbMessage *messages, uint16_t numMessages) {

}

void dinoDispose(Dino * dino) {
    dino->sprite->dispose = 1;

    if(--dinoCount == 0) {
        gbTextureUnload(dinoTexture);
        gbAnimationUnload(dinoAnimationIdle);
    }
}

static void setState(Dino *dino, DINO_STATE state) {
    dino->state = state;
    gbAnimationStateInit(dinoAnimations[state], &dino->sprite->src, &dino->animState);
}

static void idle(Dino *dino) {
    handleMoveKeyDown(dino);
}

static void speedUp(Dino *dino) {
    handleMoveKeyUp(dino);

    if (dino->direction == 0) {
        setState(dino, DINO_STATE_SLOW_DOWN);
    }

    if (fabs(dino->velocity) >= maxVelocity * runPoint) {
        setState(dino, DINO_STATE_RUN);
    }
}

static void slowDown(Dino *dino) {
    handleMoveKeyDown(dino);

    if (dino->direction == 0) {
        if (dino->velocity == 0) {
            setState(dino, DINO_STATE_IDLE);
        }
    } else {
        setState(dino, DINO_STATE_SPEED_UP);
    }
}

static void run(Dino *dino) {
    handleMoveKeyUp(dino);

    if (fabs(dino->velocity) < maxVelocity * runPoint) {
        setState(dino, DINO_STATE_SLOW_DOWN);
    }
}

static void handleMoveKeyDown(Dino *dino) {
    if (dino->velocity) {
        return;
    }

    if (dino->direction == 0 && gbInputCheckState(GB_INPUT_MOVE_RIGHT, GB_INPUT_PRESSED)) {
        dino->direction = 1;
        dino->acceleration = walkAcceleration;
        dino->sprite->flip = SDL_FLIP_NONE;
        setState(dino, DINO_STATE_SPEED_UP);
    }

    if (dino->direction == 0 && gbInputCheckState(GB_INPUT_MOVE_LEFT, GB_INPUT_PRESSED)) {
        dino->direction = -1;
        dino->acceleration = -walkAcceleration;
        dino->sprite->flip = SDL_FLIP_HORIZONTAL;
        setState(dino, DINO_STATE_SPEED_UP);
    }
}

static void handleMoveKeyUp(Dino *dino) {
    if (dino->direction == 1 && gbInputCheckState(GB_INPUT_MOVE_RIGHT, GB_INPUT_RELEASED)) {
        dino->acceleration = -stopAcceleration;
        dino->direction = 0;
    } else if (dino->direction == -1 && gbInputCheckState(GB_INPUT_MOVE_LEFT, GB_INPUT_RELEASED)) {
        dino->acceleration = stopAcceleration;
        dino->direction = 0;
    }
}

static void initDino() {
        dinoTexture = gbTextureLoad("./assets/dino.png");
        dinoAnimations[DINO_STATE_IDLE] = gbAnimationNew(0, 0, 24, 0, 4, 3, 1, GB_ANIM_TYPE_LOOP);
        dinoAnimations[DINO_STATE_SPEED_UP] = gbAnimationNew(4, 0, 24, 0, 8, 6, 1, GB_ANIM_TYPE_LOOP);
        dinoAnimations[DINO_STATE_SLOW_DOWN] = gbAnimationNew(11, 0, 24, 0, 0, 1, 1, GB_ANIM_TYPE_LOOP);
        dinoAnimations[DINO_STATE_RUN] = gbAnimationNew(18, 0, 24, 0, 8, 6, 1, GB_ANIM_TYPE_LOOP);

        gbEntityThink[GB_ENTITY_TYPE_DINO] = (GB_ENTITY_THINK_FUNC)&dinoThink;
        gbEntityRespond[GB_ENTITY_TYPE_DINO] = (GB_ENTITY_RESPOND_FUNC)&dinoRespond;
        gbEntityDispose[GB_ENTITY_TYPE_DINO] = (GB_ENTITY_DISPOSE_FUNC)&dinoDispose;
}
