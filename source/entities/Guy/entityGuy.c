#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "./entityGuy.h"

#include "../../gbUtils/gbUtils.h"

#include "../../gbRenderer/gbRenderer_sys.h"
#include "../../Collision/Collision_sys.h"
#include "../../gbGfx/gbGfx_sys.h"
#include "../../Collision/CollisionDynamicRect_type.h"

#include "../../Position_type.h"
#include "../../gbInput/gbInput_sys.h"
#include "../../gbInput/gbInputState_type.h"
#include "../../gbAnimation/gbAnimation_sys.h"
#include "../../gbAnimation/gbAnimation_type.h"
#include "../../gbAnimation/gbAnimType_type.h"

#include "../../DynamicEntity/DynamicEntityState_union.h"
#include "../../DynamicEntity/DynamicEntity_sys.h"

#include "../../Sprite/Sprite_sys.h"

#include "../../gbTexture/gbTextureName_enum.h"

static int guyTexture = -1;
static unsigned int guyAnimationIdle = GB_ANIMATION_NO_ANIMATION;

static unsigned int guyAnimations[GUY_STATE_NUM_STATES] = {
    GB_ANIMATION_NO_ANIMATION
};

static unsigned int guyCount = 0;

// CONSTS
static const double walkAcceleration = GB_LOGICAL_SCREEN_WIDTH / 10;
static const double stopAcceleration = GB_LOGICAL_SCREEN_WIDTH / 5;
static const double maxVelocity = GB_LOGICAL_SCREEN_WIDTH / 10;
static const double dropVelocity = GB_LOGICAL_SCREEN_HEIGHT / 2;
static const uint8_t HIT_GROUND = COLLISION_TOP | COLLISION_Y_MARKED;

// for Debug need to add to entity
static uint8_t grounded = 1;

// Forward declarations
static void idle(DynamicEntity *guy);
static void walk(DynamicEntity *guy, double delta);
static void handleInput(DynamicEntity *guy);
static void handleMoveKeyDown(DynamicEntity *guy);
static void handleMoveKeyUp(DynamicEntity *guy);
static void setState(DynamicEntity *guy, GUY_STATE state);

DynamicEntity *guyNew(double x, double y) {
    guyCount++;
    printf("Guy count: %d\n", guyCount);

    DynamicEntity *guy = dynamicEntityNew(DYNAMIC_ENTITY_TYPE_GUY);
    guy->pos = (Position){x, y};

    spriteSet(&guy->sprite, GB_TEXTURE_NAME_GUY, 0, 0, 32, 32, 32, 32, 1, 0, SDL_FLIP_NONE);
    spriteRegister(&guy->sprite, &guy->pos, SPRITE_LAYER_MIDGROUND);

    dynamicEntityRegister(guy);

    guy->dx = 0;
    guy->dy = 0;
    guy->ax = 0;
    guy->ay = 0;

    guy->state.guy.direction = 0;
    guy->state.guy.moveKeysDown = 0;

    guy->state.guy.state = GUY_STATE_IDLE;
    gbAnimationStateInit(guyAnimations[GUY_STATE_IDLE], &guy->sprite.src, &guy->animState);

    collisionDynamicRectSet(&guy->boundingBox, guy->id, 5, 0, 22, 31);
    collisionDynamicRectRegister(&guy->boundingBox, &guy->pos);

    return guy;
}

//void handleInput(Guy *guy) {
//    switch (guy->state) {
//        case GUY_STATE_WALK:
//        case GUY_STATE_IDLE:
//            if (gbInputCheckState(GB_INPUT_MOVE_LEFT, GB_INPUT_JUST_PRESSED) && !guy->moveKeysDown) {
//                guy->moveKeysDown = 1;
//                guy->sprite->flip = 1;
//                guy->state = GUY_STATE_WALK;
//                gbAnimationStateInit(guyAnimations[GUY_STATE_WALK], &guy->sprite->src, &guy->animState);
//                guy->ax = -walkAcceleration;
//            } else if (gbInputCheckState(GB_INPUT_MOVE_RIGHT, GB_INPUT_JUST_PRESSED) && !guy->moveKeysDown) {
//                guy->moveKeysDown = 1;
//                guy->sprite->flip = 0;
//                guy->state = GUY_STATE_WALK;
//                gbAnimationStateInit(guyAnimations[GUY_STATE_WALK], &guy->sprite->src, &guy->animState);
//                guy->ax = walkAcceleration;
//            }
//
//            if (gbInputCheckState(GB_INPUT_MOVE_LEFT, GB_INPUT_RELEASED) && guy->moveKeysDown) {
//                guy->moveKeysDown = 0;
//                guy->ax = stopAcceleration;
//            } else if (gbInputCheckState(GB_INPUT_MOVE_RIGHT, GB_INPUT_RELEASED) && guy->moveKeysDown) {
//                guy->moveKeysDown = 0;
//                guy->ax = -stopAcceleration;
//            }
//        break;
//    }
//}

void guyThink(DynamicEntity *guy, double delta) {
    // INPUT
    int xMove = gbInputCheckState(GB_INPUT_MOVE_LEFT, GB_INPUT_JUST_PRESSED) ? -1   :
                gbInputCheckState(GB_INPUT_MOVE_RIGHT, GB_INPUT_JUST_PRESSED) ? 1   :
                0;

    if (xMove) {
        guy->sprite.flip = xMove < 0;
        setState(guy, GUY_STATE_WALK);
    } else {
        xMove = gbInputCheckState(GB_INPUT_MOVE_LEFT, GB_INPUT_PRESSED) ? -1   :
                gbInputCheckState(GB_INPUT_MOVE_RIGHT, GB_INPUT_PRESSED) ? 1   :
                0;
    }

    if (xMove) {
        guy->ax = xMove * walkAcceleration;
    } else {
        xMove = gbInputCheckState(GB_INPUT_MOVE_LEFT, GB_INPUT_RELEASED) ? -1 :
                gbInputCheckState(GB_INPUT_MOVE_RIGHT, GB_INPUT_RELEASED) ? 1 :
                0;

        if (xMove) {
            if ((xMove > 0) - (xMove < 0) != (guy->dx > 0) - (guy->dx < 0)) {
                guy->ax = xMove > 0 ? stopAcceleration : -stopAcceleration;
            } else {
                guy->ax = xMove > 0 ? -stopAcceleration : stopAcceleration;
            }
        }
    }

    if (grounded) {
        if (gbInputCheckState(GB_INPUT_JUMP, GB_INPUT_PRESSED)) {
            guy->dy = -250;
        }
    }

    // MOVEMENT PHYSICS
    double oldVelocity = guy->dx;
    guy->dx += (guy->ax * delta);

    if (guy->dx > maxVelocity) {
        guy->dx = maxVelocity;
    } else if (guy->dx < -maxVelocity) {
        guy->dx = -maxVelocity;
    }

    int oldSign = (int)(oldVelocity > 0) - (oldVelocity < 0);
    int newSign = (int)(guy->dx > 0) - (guy->dx < 0);

    oldSign = !oldSign ? 1 : oldSign;
    newSign = !newSign ? 1 : newSign;

    if (!xMove && oldVelocity != 0 && guy->dx != 0 && oldSign + newSign == 0) {
        guy->dx = 0;
        guy->ax = 0;
        guy->state.guy.state = GUY_STATE_IDLE;
        gbAnimationStateInit(guyAnimations[GUY_STATE_IDLE], &guy->sprite.src, &guy->animState);
    }

    switch (guy->state.guy.state) {
        case GUY_STATE_IDLE:
            idle(guy);
            break;
        case GUY_STATE_WALK:
            walk(guy, delta);
            break;
    }

    guy->pos.y += guy->dy * delta;
    guy->dy += dropVelocity * delta;

    gbAnimationApply(&guy->sprite.src, delta, &guy->animState, guyAnimations[guy->state.guy.state]);

    unsigned int index = 0;
    uint8_t collData = 0;

    grounded = 0;

    while (index = collisionResolveStaticCollisions(index, &guy->boundingBox, guy->dx, guy->dy, &collData)) {
        if ((collData & HIT_GROUND) == HIT_GROUND) {
            if (guy->dy > 0) {
                guy->dy = 0;
                grounded = 1;
            }
        }
    }
}

void guyRespond(DynamicEntity *guy, double delta) {

}

//void guyDispose(DynamicEntity * guy) {
//    guy->sprite->dispose = 1;
//    guy->boundingBox->dispose = 1;
//
//    if(--guyCount == 0) {
//        gbTextureUnload(guyTexture);
//        gbAnimationUnload(guyAnimationIdle);
//    }
//
//    printf("guyCount: %d", guyCount);
//}

static void setState(DynamicEntity *guy, GUY_STATE state) {
    guy->state.guy.state = state;

    gbAnimationStateInit(guyAnimations[state], &guy->sprite.src, &guy->animState);
}

static void idle(DynamicEntity *guy) {
    //handleMoveKeyDown(guy);
}

static void walk(DynamicEntity *guy, double delta) {
   guy->pos.x += guy->dx * delta;
}

//static void handleMoveKeyDown(Guy *guy) {
//    if (guy->dx) {
//        return;
//    }
//
//    if (guy->direction == 0 && gbInputCheckState(GB_INPUT_MOVE_RIGHT, GB_INPUT_PRESSED)) {
//        guy->direction = 1;
//        guy->ax = walkAcceleration;
//        guy->sprite->flip = SDL_FLIP_NONE;
//        setState(guy, GUY_STATE_WALK);
//    }
//
//    if (guy->direction == 0 && gbInputCheckState(GB_INPUT_MOVE_LEFT, GB_INPUT_PRESSED)) {
//        guy->direction = -1;
//        guy->ax = -walkAcceleration;
//        guy->sprite->flip = SDL_FLIP_HORIZONTAL;
//        setState(guy, GUY_STATE_WALK);
//    }
//}
//
//static void handleMoveKeyUp(Guy *guy) {
//    if (guy->direction == 1 && gbInputCheckState(GB_INPUT_MOVE_RIGHT, GB_INPUT_RELEASED)) {
//        guy->ax = -stopAcceleration;
//        guy->direction = 0;
//    } else if (guy->direction == -1 && gbInputCheckState(GB_INPUT_MOVE_LEFT, GB_INPUT_RELEASED)) {
//        guy->ax = stopAcceleration;
//        guy->direction = 0;
//    }
//}


void guyInit() {
        guyTexture = gbTextureLoadNamed(GB_TEXTURE_NAME_GUY);
        guyAnimations[GUY_STATE_IDLE] = gbAnimationNew(0, 0, 32, 0, 8, 1, 1, GB_ANIM_TYPE_LOOP);
        guyAnimations[GUY_STATE_WALK] = gbAnimationNew(0, 0, 32, 0, 8, 8, 1, GB_ANIM_TYPE_LOOP);
}

void guyTeardown() {
    gbTextureUnload(GB_TEXTURE_NAME_GUY);
    gbAnimationUnload(guyAnimations[GUY_STATE_IDLE]);
    gbAnimationUnload(guyAnimations[GUY_STATE_WALK]);
}
