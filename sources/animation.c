#include <stdlib.h>
#include <stdint.h>

#include <SDL2/SDL.h>

#include "../headers/animation.h"

GbAnimation *gb_anim_new_animation(
                                   uint16_t init_x,
                                   uint16_t init_y,
                                   uint16_t dx,
                                   uint16_t dy,
                                   uint16_t interval,
                                   uint16_t frame_count,
                                   int8_t direction,
                                   GB_ANIM_TYPE type) {
   GbAnimation *new_animation = (GbAnimation *)malloc(sizeof(GbAnimation));

   new_animation->init_x = init_x;
   new_animation->init_y = init_y;
   new_animation->dx = dx;
   new_animation->dy = dy;
   new_animation->interval = interval;
   new_animation->frame_count = frame_count;
   new_animation->direction = direction;
   new_animation->type = type;

   new_animation->current_frame = 0;
   new_animation->accumulator = 0;

   return new_animation;
}

uint16_t gb_anim_apply(SDL_Rect *src, uint32_t delta, GbAnimation *animation) {
    animation->accumulator += delta;

    if (animation->accumulator < animation->interval) {
        return animation->current_frame;
    }

    animation->accumulator -= animation->interval;

    if (animation->type == ANIM_TYPE_LOOP) {
        if (animation->direction > 0 && animation->current_frame == animation->frame_count - 1) {
            animation->current_frame = 0;
        } else if (animation->direction < 0 && animation->current_frame == 0) {
            animation->current_frame = animation->frame_count - 1;
        } else {
            animation->current_frame += animation->direction;
        }
    } else if (animation->type == ANIM_TYPE_PINGPONG) {
        if ((animation->direction > 0 && animation->current_frame == animation->frame_count - 1) ||
            (animation->direction < 0 && animation->current_frame == 0)) {
            animation->direction = -animation->direction;
        }

        animation->current_frame += animation->direction;
    }

    src->x = animation->dx * animation->current_frame;
    src->y = animation->dy * animation->current_frame;

    return animation->current_frame;
}
