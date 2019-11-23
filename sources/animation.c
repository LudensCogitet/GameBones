#include <stdlib.h>
#include <stdint.h>

#include <SDL2/SDL.h>

#include "../headers/animation.h"

GbAnimation *gb_anim_new_animation(
                                   uint16_t init_x,
                                   uint16_t init_y,
                                   uint16_t dx,
                                   uint16_t dy,
                                   double interval,
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
   new_animation->direction = new_animation->init_direction = direction;
   new_animation->type = type;

   new_animation->current_frame = new_animation->direction > 0 ? 0 : new_animation->frame_count - 1;
   new_animation->accumulator = 0;

   return new_animation;
}

uint8_t gb_anim_apply(SDL_Rect *src, double delta, GbAnimation *animation) {
    animation->accumulator += delta;

    if (animation->accumulator < animation->interval) {
        return 0;
    }

    animation->accumulator -= animation->interval;

    uint8_t anim_finished = 0;

    if (animation->direction > 0 && animation->current_frame == animation->frame_count - 1) {
        animation->current_frame = 0;
    } else if (animation->direction < 0 && animation->current_frame == 0) {
        animation->current_frame = animation->frame_count - 1;
    } else {
        animation->current_frame += animation->direction;
    }

    if (animation->type == ANIM_TYPE_LOOP) {
        anim_finished = (animation->init_direction > 0 && animation->current_frame == animation->frame_count -1) ||
                        (animation->init_direction < 0 && animation->current_frame == 0);
    } else if (animation->type == ANIM_TYPE_PINGPONG) {
        if ((animation->current_frame == animation->frame_count - 1 && animation->direction > 0) ||
            (animation->current_frame == 0 && animation->direction < 0)) {
            animation->direction = -animation->direction;
        }

        anim_finished = (animation->init_direction > 0 && animation->direction == animation->init_direction && animation->current_frame == 0) ||
                        (animation->init_direction < 0 && animation->direction == animation->init_direction && animation->current_frame == animation->frame_count -1);
    }

    if (animation->dx) src->x = animation->init_x + (animation->dx * animation->current_frame);
    if (animation->dy) src->y = animation->init_y + (animation->dy * animation->current_frame);

    return anim_finished;
}
