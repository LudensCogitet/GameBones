#ifndef BULLET_H_INCLUDED
#define BULLET_H_INCLUDED

#include "../gfx.h"
#include "../physics.h"
#include "../entity.h"
#include "../message.h"

typedef struct Bullet {
    GbSprite *sprite;
    GbPhysBod *body;
    GbEntity *self;

    double lifespan;
    double accumulator;
} Bullet;

Bullet *bullet_new(float x, float y, double dx, double dy, double lifespan);
void bullet_destroy(Bullet *bullet);
void bullet_act(Bullet *bullet, double delta);
void bullet_handle_message(Bullet *bullet, GbMessage message);

#endif
