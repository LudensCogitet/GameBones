#ifndef ASTEROID_H_INCLUDED
#define ASTEROID_H_INCLUDED

#include "../gfx.h"
#include "../animation.h"
#include "../physics.h"

typedef struct Asteroid {
    GbAnimation *anim_rotate;

    GbSprite *sprite;
    GbPhysBod *body;
} Asteroid;

Asteroid *asteroid_new(float x, float y, unsigned int dir);
void asteroid_destroy(Asteroid *asteroid);
void asteroid_act(Asteroid *asteroid, uint32_t delta);

#endif
