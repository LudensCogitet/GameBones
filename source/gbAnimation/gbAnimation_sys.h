#ifndef GB_ANIMATION_SYSTEM_H_INCLUDED
#define GB_ANIMATION_SYSTEM_H_INCLUDED

#include "./gbAnimation_type.h"
#include "./gbAnimType_type.h"
#include "../gbGfx/gbSprite/gbSprite_type.h"

#define GB_ANIMATION_MAX_ANIMATIONS 100
#define GB_ANIMATION_NO_ANIMATION GB_ANIMATION_MAX_ANIMATIONS + 1

void gbAnimationInit();
void gbAnimationTeardown();

unsigned int gbAnimationNew(
                        uint16_t initX,
                        uint16_t initY,

                        uint16_t dx,
                        uint16_t dy,

                        double fps,
                        uint16_t frameCount,

                        int8_t direction,
                        GB_ANIM_TYPE type);

void gbAnimationUnload(unsigned int index);

void gbAnimationStateInit(unsigned int animation, SDL_Rect *src, gbAnimationState *state);

uint8_t gbAnimationApply(SDL_Rect *src, double delta, gbAnimationState *state, unsigned int animation);

#endif // GB_ANIMATION_SYSTEM_H_INCLUDED
