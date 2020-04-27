#ifndef GB_ANIMATION_TYPE_H_INCLUDED
#define GB_ANIMATION_TYPE_H_INCLUDED

#include "./gbAnimType_type.h"

typedef struct {
    uint16_t initX;
    uint16_t initY;

    uint16_t dx;
    uint16_t dy;

    double interval;

    uint16_t frameCount;
    int8_t initDirection;

    GB_ANIM_TYPE type;
} gbAnimation;

typedef struct {
    double accumulator;
    uint16_t currentFrame;
    int8_t direction;
} gbAnimationState;

#endif // GB_ANIMATION_TYPE_H_INCLUDED
