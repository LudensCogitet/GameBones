#include <stdlib.h>
#include <stdint.h>

#include <SDL2/SDL.h>

#include "./gbAnimation_sys.h"
#include "./gbAnimation_type.h"

#include "../gbResourceManagement/gbResManagement.h"

static gbAnimation *animations[GB_ANIMATION_MAX_ANIMATIONS];
static unsigned int cursor;

void gbAnimationInit() {
    for (unsigned int i = 0; i < GB_ANIMATION_MAX_ANIMATIONS; i++) {
        animations[i] = 0;
    }
}

void gbAnimationTeardown() {
    for (unsigned int i = 0; i < GB_ANIMATION_MAX_ANIMATIONS; i++) {
        free(animations[i]);
        animations[i] = 0;
    }

    cursor = 0;
}

unsigned int gbAnimationNew(
                                uint16_t initX,
                                uint16_t initY,

                                uint16_t dx,
                                uint16_t dy,

                                double fps,
                                uint16_t frameCount,

                                int8_t direction,
                                GB_ANIM_TYPE type) {

    cursor = gbNextEmpty(animations, cursor, GB_ANIMATION_MAX_ANIMATIONS);
    if (cursor == GB_ANIMATION_MAX_ANIMATIONS) { return 0; }

    animations[cursor] = (gbAnimation *)malloc(sizeof(gbAnimation));

    animations[cursor]->initX = initX * dx;
    animations[cursor]->initY = initY * dy;
    animations[cursor]->dx = dx;
    animations[cursor]->dy = dy;
    animations[cursor]->interval = 1 / fps;
    animations[cursor]->frameCount = frameCount;
    animations[cursor]->initDirection = direction;
    animations[cursor]->type = type;

    return cursor;
}

void gbAnimationUnload(unsigned int index) {
    if (animations[index] != 0) {
        free(animations[index]);
        animations[index] = 0;
    }
}

void gbAnimationStateInit(unsigned int animation, SDL_Rect *src, gbAnimationState *state) {
    state->accumulator = 0;
    state->direction = animations[animation]->initDirection;
    state->currentFrame = animations[animation]->initDirection > 0 ? 0 : animations[animation]->frameCount - 1;

    if (animations[animation]->dx) src->x = animations[animation]->initX + (animations[animation]->dx * state->currentFrame);
    if (animations[animation]->dy) src->y = animations[animation]->initY + (animations[animation]->dy * state->currentFrame);
}

uint8_t gbAnimationApply(SDL_Rect *src, double delta, gbAnimationState *state, unsigned int animation) {
    state->accumulator += delta;

    if (state->accumulator < animations[animation]->interval) {
        return 0;
    }

    state->accumulator -= animations[animation]->interval;

    uint8_t animFinished = 0;

    if (state->direction > 0 && state->currentFrame == animations[animation]->frameCount - 1) {
        state->currentFrame = 0;
    } else if (state->direction < 0 && state->currentFrame == 0) {
        state->currentFrame = animations[animation]->frameCount - 1;
    } else {
        state->currentFrame += state->direction;
    }

    if (animations[animation]->type == GB_ANIM_TYPE_LOOP) {
        animFinished = (animations[animation]->initDirection > 0 && state->currentFrame == animations[animation]->frameCount -1) ||
                        (animations[animation]->initDirection < 0 && state->currentFrame == 0);
    } else if (animations[animation]->type == GB_ANIM_TYPE_PINGPONG) {
        if ((state->currentFrame == animations[animation]->frameCount - 1 && state->direction > 0) ||
            (state->currentFrame == 0 && state->direction < 0)) {
            state->direction = -state->direction;
        }

        animFinished = (animations[animation]->initDirection > 0 && state->direction == animations[animation]->initDirection && state->currentFrame == 0) ||
                        (animations[animation]->initDirection < 0 && state->direction == animations[animation]->initDirection && state->currentFrame == animations[animation]->frameCount -1);
    }

    if (animations[animation]->dx) src->x = animations[animation]->initX + (animations[animation]->dx * state->currentFrame);
    if (animations[animation]->dy) src->y = animations[animation]->initY + (animations[animation]->dy * state->currentFrame);

    return animFinished;
}
