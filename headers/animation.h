
#ifndef ANIMATION_H_INCLUDED
#define ANIMATION_H_INCLUDED

#include "SDL2/SDL.h"

typedef enum GB_ANIM_TYPE {
    ANIM_TYPE_LOOP,
    ANIM_TYPE_PINGPONG,
} GB_ANIM_TYPE;

typedef struct GbAnimation {
    uint16_t init_x;
    uint16_t init_y;

    uint16_t dx;
    uint16_t dy;

    double interval;
    double accumulator;

    uint16_t frame_count;
    uint16_t current_frame;
    int8_t direction;
    int8_t init_direction;

    GB_ANIM_TYPE type;
} GbAnimation;

GbAnimation *gb_anim_new_animation(
                                   uint16_t init_x,
                                   uint16_t init_y,
                                   uint16_t dx,
                                   uint16_t dy,
                                   double interval,
                                   uint16_t frame_count,
                                   int8_t direction,
                                   GB_ANIM_TYPE type);

uint8_t gb_anim_apply(SDL_Rect *src, double delta, GbAnimation *animation);
#endif // ANIMATION_H_INCLUDED
