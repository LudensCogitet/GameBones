#ifndef PLAYER_SHIP_H_INCLUDED
#define PLAYER_SHIP_H_INCLUDED

#include "../gfx.h"
#include "../animation.h"
#include "../physics.h"
#include "../message.h"

typedef struct PlayerShip {
    GbAnimation *anim_rotate_ship;
    GbAnimation *anim_boost;
    GbAnimation *anim_thrust;

    GbSprite *sprite;
    GbPhysBod *body;

    float acceleration;
    float boostAcceleration;

    uint8_t boosting;
    uint8_t thrusting;

    int sfx_cruise_channel;
} PlayerShip;

PlayerShip *player_ship_new(float x, float y, unsigned int dir, float acc, float boostAcc);
void player_ship_destroy(PlayerShip *ship);
void player_ship_act(PlayerShip *ship, double delta);
void player_ship_handle_messages(PlayerShip *ship, GbMessage *messages, unsigned int numMessages);

#endif
