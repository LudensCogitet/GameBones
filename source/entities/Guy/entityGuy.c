#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "./entityGuy.h"

#include "../../gbRenderer/gbRenderer_sys.h"
#include "../../gbCollision/gbCollision_sys.h"
#include "../../gbGfx/gbGfx_sys.h"
#include "../../gbCollision/gbCollisionDynamicRect_type.h"

#include "../../gbEntity/gbPosition_type.h"
#include "../../gbInput/gbInput_sys.h"
#include "../../gbInput/gbInputState_type.h"
#include "../../gbAnimation/gbAnimation_sys.h"
#include "../../gbAnimation/gbAnimation_type.h"
#include "../../gbAnimation/gbAnimType_type.h"

#include "../../gbSerializer/gbSerializer_sys.h"
#include "../../gbSerializer/gbFile_type.h"
#include "../../gbSerializer/gbFileChunkSize_type.h"

static unsigned int guyTexture = GB_TEXTURE_NO_TEXTURE;
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
static const uint8_t HIT_GROUND = GB_COLLISION_TOP | GB_COLLISION_Y_MARKED;

// for Debug need to add to entity
static uint8_t grounded = 1;

// Forward declarations
static void idle(Guy *guy);
static void walk(Guy *guy);
static void handleInput(Guy *guy);
static void handleMoveKeyDown(Guy *guy);
static void handleMoveKeyUp(Guy *guy);

gbEntity *guyNew(double x, double y, SDL_RendererFlip flip) {
    printf("Guy count: %d\n", guyCount);

    Guy *guy = (Guy *)malloc(sizeof(Guy));
    guy->pos = (gbPosition){x, y};

    gbSprite *sprite = gbGfxSpriteNew(
                                      GB_GFX_LAYER_MIDGROUND,
                                      guyTexture,
                                      0, 0, 32, 32,
                                      &guy->pos, 32, 32,
                                      0,
                                      flip);
    guy->sprite = sprite;

    guy->dx = 0;
    guy->dy = 0;
    guy->ax = 0;
    guy->ay = 0;

    guy->direction = 0;
    guy->moveKeysDown = 0;

    guy->state = GUY_STATE_IDLE;
    gbAnimationStateInit(guyAnimations[GUY_STATE_IDLE], &sprite->src, &guy->animState);

    guy->entity = gbEntityNew(GB_ENTITY_TYPE_GUY, guy, GB_ENTITY_PRIORITY_HIGH);

    guy->boundingBox = gbCollisionDynamicColliderNew(&guy->pos, guy->entity, 5, 0, 22, 31);

    return guy->entity;
}

void handleInput(Guy *guy) {
    switch (guy->state) {
        case GUY_STATE_WALK:
        case GUY_STATE_IDLE:
            if (gbInputCheckState(GB_INPUT_MOVE_LEFT, GB_INPUT_JUST_PRESSED) && !guy->moveKeysDown) {
                guy->moveKeysDown = 1;
                guy->sprite->flip = 1;
                guy->state = GUY_STATE_WALK;
                gbAnimationStateInit(guyAnimations[GUY_STATE_WALK], &guy->sprite->src, &guy->animState);
                guy->ax = -walkAcceleration;
            } else if (gbInputCheckState(GB_INPUT_MOVE_RIGHT, GB_INPUT_JUST_PRESSED) && !guy->moveKeysDown) {
                guy->moveKeysDown = 1;
                guy->sprite->flip = 0;
                guy->state = GUY_STATE_WALK;
                gbAnimationStateInit(guyAnimations[GUY_STATE_WALK], &guy->sprite->src, &guy->animState);
                guy->ax = walkAcceleration;
            }

            if (gbInputCheckState(GB_INPUT_MOVE_LEFT, GB_INPUT_RELEASED) && guy->moveKeysDown) {
                guy->moveKeysDown = 0;
                guy->ax = stopAcceleration;
            } else if (gbInputCheckState(GB_INPUT_MOVE_RIGHT, GB_INPUT_RELEASED) && guy->moveKeysDown) {
                guy->moveKeysDown = 0;
                guy->ax = -stopAcceleration;
            }
        break;
    }
}

void guyThink(Guy *guy, double delta) {
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

    if (oldVelocity != 0 && guy->dx != 0 && oldSign + newSign == 0) {
        guy->dx = 0;
        guy->ax = 0;
        guy->state = GUY_STATE_IDLE;
        gbAnimationStateInit(guyAnimations[GUY_STATE_IDLE], &guy->sprite->src, &guy->animState);
    }

    guy->pos.x += guy->dx * delta;
    guy->pos.y += guy->dy * delta;

    if (!grounded) {
        guy->dy += dropVelocity * delta;
    }

    gbAnimationApply(&guy->sprite->src, delta, &guy->animState, guyAnimations[guy->state]);

    switch (guy->state) {
        case GUY_STATE_IDLE:
            idle(guy);
            break;
        case GUY_STATE_WALK:
            walk(guy);
            break;
    }

    if (grounded) {
        if (gbInputCheckState(GB_INPUT_JUMP, GB_INPUT_JUST_PRESSED)) {
            grounded = 0;
            guy->dy = -250;
        }
    }

    if (!grounded || guy->state == GUY_STATE_WALK) {
        unsigned int index = 0;
        uint8_t collData;

        while (index = gbCollisionResolveStaticCollisions(index, guy->boundingBox, guy->dx, guy->dy, &collData)) {
            if (collData & HIT_GROUND) {
                grounded = 1;
                guy->dy = 0;
            }
        }
    }
}

void guyRespond(Guy *guy, gbMessage *messages, uint16_t numMessages) {

}

void guyDispose(Guy * guy) {
    guy->sprite->dispose = 1;
    guy->boundingBox->dispose = 1;

    if(--guyCount == 0) {
        gbTextureUnload(guyTexture);
        gbAnimationUnload(guyAnimationIdle);
    }
}

static void setState(Guy *guy, GUY_STATE state) {
    guy->state = state;

    if (state == GUY_STATE_IDLE || state == GUY_STATE_WALK) {
        gbAnimationStateInit(guyAnimations[state], &guy->sprite->src, &guy->animState);
    }
}

static void idle(Guy *guy) {
    handleMoveKeyDown(guy);
}

static void walk(Guy *guy) {
    handleMoveKeyUp(guy);
}

static void handleMoveKeyDown(Guy *guy) {
    if (guy->dx) {
        return;
    }

    if (guy->direction == 0 && gbInputCheckState(GB_INPUT_MOVE_RIGHT, GB_INPUT_PRESSED)) {
        guy->direction = 1;
        guy->ax = walkAcceleration;
        guy->sprite->flip = SDL_FLIP_NONE;
        setState(guy, GUY_STATE_WALK);
    }

    if (guy->direction == 0 && gbInputCheckState(GB_INPUT_MOVE_LEFT, GB_INPUT_PRESSED)) {
        guy->direction = -1;
        guy->ax = -walkAcceleration;
        guy->sprite->flip = SDL_FLIP_HORIZONTAL;
        setState(guy, GUY_STATE_WALK);
    }
}

static void handleMoveKeyUp(Guy *guy) {
    if (guy->direction == 1 && gbInputCheckState(GB_INPUT_MOVE_RIGHT, GB_INPUT_RELEASED)) {
        guy->ax = -stopAcceleration;
        guy->direction = 0;
    } else if (guy->direction == -1 && gbInputCheckState(GB_INPUT_MOVE_LEFT, GB_INPUT_RELEASED)) {
        guy->ax = stopAcceleration;
        guy->direction = 0;
    }
}

void guySerialize(Guy * guy, gbFile *file) {
    // Write entity type
    gbSerializerWriteChunk(file, GB_FILE_CHUNK_SIZE_16, GB_ENTITY_TYPE_GUY);

    // Write x and y coordinates
    gbSerializerWriteChunk(file, GB_FILE_CHUNK_SIZE_64, guy->pos.x);
    gbSerializerWriteChunk(file, GB_FILE_CHUNK_SIZE_64, guy->pos.y);

    // Write flipped
    gbSerializerWriteChunk(file, GB_FILE_CHUNK_SIZE_8, guy->sprite->flip);
}
Guy *guyDeserialize(gbFile *file) {
    double x = gbSerializerReadChunk(file, GB_FILE_CHUNK_SIZE_64);
    double y = gbSerializerReadChunk(file, GB_FILE_CHUNK_SIZE_64);
    uint8_t flip = gbSerializerReadChunk(file, GB_FILE_CHUNK_SIZE_8);

    return guyNew(x, y, flip)->entity;
}

void initGuy() {
        guyTexture = gbTextureLoad("./assets/red_guy.png");
        guyAnimations[GUY_STATE_IDLE] = gbAnimationNew(0, 0, 32, 0, 8, 1, 1, GB_ANIM_TYPE_LOOP);
        guyAnimations[GUY_STATE_WALK] = gbAnimationNew(0, 0, 32, 0, 8, 8, 1, GB_ANIM_TYPE_LOOP);

        gbEntityRegisterFuncs(
                              GB_ENTITY_TYPE_GUY,
                              (GB_ENTITY_THINK_FUNC)&guyThink,
                              (GB_ENTITY_RESPOND_FUNC)&guyRespond,
                              (GB_ENTITY_DISPOSE_FUNC)&guyDispose,
                              (GB_ENTITY_SERIALIZE_FUNC)&guySerialize,
                              (GB_ENTITY_DESERIALIZE_FUNC)&guyDeserialize
                              );
}
