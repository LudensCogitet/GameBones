#include <stdlib.h>
#include <stdint.h>

#include <SDL2/SDL.h>

#include "./gbAnimation_sys.h"
#include "./gbAnimation_type.h"

gbAnimation *gbAnimations[GB_ANIMATION_MAX_ANIMATIONS];
static unsigned int gbAnimationsCursor;

void gbAnimationInit() {
    for (unsigned int i = 0; i < GB_ANIMATION_MAX_ANIMATIONS; i++) {
        gbAnimations[i] = 0;
    }
}

void gbAnimationTeardown() {
    for (unsigned int i = 0; i < GB_ANIMATION_MAX_ANIMATIONS; i++) {
        free(gbAnimations[i]);
        gbAnimations[i] = 0;
    }

    gbAnimationsCursor = 0;
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

    if (gbAnimationsCursor >= GB_ANIMATION_MAX_ANIMATIONS) { return 0; }

    gbAnimations[gbAnimationsCursor] = (gbAnimation *)malloc(sizeof(gbAnimation));

    gbAnimations[gbAnimationsCursor]->initX = initX * dx;
    gbAnimations[gbAnimationsCursor]->initY = initY * dy;
    gbAnimations[gbAnimationsCursor]->dx = dx;
    gbAnimations[gbAnimationsCursor]->dy = dy;
    gbAnimations[gbAnimationsCursor]->interval = 1 / fps;
    gbAnimations[gbAnimationsCursor]->frameCount = frameCount;
    gbAnimations[gbAnimationsCursor]->initDirection = direction;
    gbAnimations[gbAnimationsCursor]->type = type;

    return gbAnimationsCursor++;
}

void gbAnimationUnload(unsigned int index) {
    if (gbAnimations[index] != 0) {
        free(gbAnimations[index]);
    }

    if (--gbAnimationsCursor > 0) {
        gbAnimations[index] = gbAnimations[gbAnimationsCursor];
        gbAnimations[gbAnimationsCursor] = 0;
    } else {
        gbAnimations[index] = 0;
    }
}

void gbAnimationStateInit(unsigned int animation, SDL_Rect *src, gbAnimationState *state) {
    state->accumulator = 0;
    state->direction = gbAnimations[animation]->initDirection;
    state->currentFrame = gbAnimations[animation]->initDirection > 0 ? 0 : gbAnimations[animation]->frameCount - 1;

    if (gbAnimations[animation]->dx) src->x = gbAnimations[animation]->initX + (gbAnimations[animation]->dx * state->currentFrame);
    if (gbAnimations[animation]->dy) src->y = gbAnimations[animation]->initY + (gbAnimations[animation]->dy * state->currentFrame);
}

uint8_t gbAnimationApply(SDL_Rect *src, double delta, gbAnimationState *state, unsigned int animation) {
    state->accumulator += delta;

    if (state->accumulator < gbAnimations[animation]->interval) {
        return 0;
    }

    state->accumulator -= gbAnimations[animation]->interval;

    uint8_t animFinished = 0;

    if (state->direction > 0 && state->currentFrame == gbAnimations[animation]->frameCount - 1) {
        state->currentFrame = 0;
    } else if (state->direction < 0 && state->currentFrame == 0) {
        state->currentFrame = gbAnimations[animation]->frameCount - 1;
    } else {
        state->currentFrame += state->direction;
    }

    if (gbAnimations[animation]->type == GB_ANIM_TYPE_LOOP) {
        animFinished = (gbAnimations[animation]->initDirection > 0 && state->currentFrame == gbAnimations[animation]->frameCount -1) ||
                        (gbAnimations[animation]->initDirection < 0 && state->currentFrame == 0);
    } else if (gbAnimations[animation]->type == GB_ANIM_TYPE_PINGPONG) {
        if ((state->currentFrame == gbAnimations[animation]->frameCount - 1 && state->direction > 0) ||
            (state->currentFrame == 0 && state->direction < 0)) {
            state->direction = -state->direction;
        }

        animFinished = (gbAnimations[animation]->initDirection > 0 && state->direction == gbAnimations[animation]->initDirection && state->currentFrame == 0) ||
                        (gbAnimations[animation]->initDirection < 0 && state->direction == gbAnimations[animation]->initDirection && state->currentFrame == gbAnimations[animation]->frameCount -1);
    }

    if (gbAnimations[animation]->dx) src->x = gbAnimations[animation]->initX + (gbAnimations[animation]->dx * state->currentFrame);
    if (gbAnimations[animation]->dy) src->y = gbAnimations[animation]->initY + (gbAnimations[animation]->dy * state->currentFrame);

    return animFinished;
}
