#include "../../headers/entities/player_ship.h"
#include "../../headers/entity.h"
#include "../../headers/input.h"
#include <stdio.h>

static float ship_direction[48][2] = {
{-0.000000, -1.000000},
{-0.130526, -0.991445},
{-0.258819, -0.965926},
{-0.382683, -0.923880},
{-0.500000, -0.866025},
{-0.608761, -0.793353},
{-0.707107, -0.707107},
{-0.793353, -0.608761},
{-0.866025, -0.500000},
{-0.923880, -0.382683},
{-0.965926, -0.258819},
{-0.991445, -0.130526},
{-1.000000, 0.000000},
{-0.991445, 0.130526},
{-0.965926, 0.258819},
{-0.923880, 0.382683},
{-0.866025, 0.500000},
{-0.793353, 0.608761},
{-0.707107, 0.707107},
{-0.608761, 0.793353},
{-0.500000, 0.866025},
{-0.382683, 0.923880},
{-0.258819, 0.965926},
{-0.130526, 0.991445},
{0.000000, 1.000000},
{0.130526, 0.991445},
{0.258819, 0.965926},
{0.382683, 0.923880},
{0.500000, 0.866025},
{0.608761, 0.793353},
{0.707107, 0.707107},
{0.793353, 0.608761},
{0.866025, 0.500000},
{0.923880, 0.382683},
{0.965926, 0.258819},
{0.991445, 0.130526},
{1.000000, 0.000000},
{0.991445, -0.130526},
{0.965926, -0.258819},
{0.923880, -0.382683},
{0.866025, -0.500000},
{0.793353, -0.608761},
{0.707107, -0.707107},
{0.608761, -0.793353},
{0.500000, -0.866025},
{0.382683, -0.923880},
{0.258819, -0.965926},
{0.130526, -0.991445}
};

PlayerShip *player_ship_new(float x, float y, unsigned int dir, float acc, float boostAcc) {
    PlayerShip *ship = (PlayerShip *)malloc(sizeof(PlayerShip));

    ship->x = x;
    ship->y = y;
    ship->dx = 0;
    ship->dy = 0;

    ship->dir = ship_direction[dir];

    ship->acceleration = acc;
    ship->boostAcceleration = boostAcc;

    ship->boosting = 0;
    ship->thrusting = 0;

    ship->sprite = gb_gfx_new_sprite(GFX_LAYER_MIDGROUND, GFX_TEXTURE_SHIP, 0);

    ship->sprite->src.x = 0;
    ship->sprite->src.y = 0;
    ship->sprite->src.w = 128;
    ship->sprite->src.h = 128;

    ship->sprite->dst.x = x - ship->sprite->dst.w * 0.5;
    ship->sprite->dst.y = y - ship->sprite->dst.h * 0.5;
    ship->sprite->dst.w = 90;
    ship->sprite->dst.h = 90;

    ship->anim_rotate_ship = gb_anim_new_animation(0, 0, 128, 0, 21, 48, 1, ANIM_TYPE_LOOP);
    ship->anim_boost = gb_anim_new_animation(0, 128, 0, 128, 50, 5, 1, ANIM_TYPE_LOOP);
    ship->anim_thrust = gb_anim_new_animation(0, 128, 0, 128, 100, 3, -1, ANIM_TYPE_PINGPONG);

    gb_entity_add(ENTITY_TYPE_PLAYER_SHIP, (void *)ship);

    return ship;
}

void player_ship_destroy(PlayerShip *ship) {
    free(ship->anim_rotate_ship);
    free(ship->anim_boost);
    free(ship->anim_thrust);

    ship->sprite->dispose = 1;

    free(ship);
}

void player_ship_act(PlayerShip *ship, uint32_t delta) {
    double dDelta = (double)(delta * 0.001);

    if (gb_input_check_state(GB_INPUT_ROTATE_LEFT, GB_INPUT_PRESSED)) {
        ship->anim_rotate_ship->direction = 1;
        gb_anim_apply(&(ship->sprite->src), delta, ship->anim_rotate_ship);
        ship->dir = ship_direction[ship->anim_rotate_ship->current_frame];
    }

    if (gb_input_check_state(GB_INPUT_ROTATE_RIGHT, GB_INPUT_PRESSED)) {
        ship->anim_rotate_ship->direction = -1;
        gb_anim_apply(&(ship->sprite->src), delta, ship->anim_rotate_ship);
        ship->dir = ship_direction[ship->anim_rotate_ship->current_frame];
    }

    if (gb_input_check_state(GB_INPUT_THRUST, GB_INPUT_JUST_PRESSED)) {
        ship->boosting = 1;
    }

    if (gb_input_check_state(GB_INPUT_THRUST, GB_INPUT_RELEASED)) {
        ship->boosting = ship->thrusting = ship->anim_thrust->current_frame = ship->anim_boost->current_frame = 0;
        ship->anim_thrust->direction = -1;
        ship->sprite->src.y = 0;
    }

    if (gb_input_check_state(GB_INPUT_THRUST, GB_INPUT_PRESSED)) {
        ship->dx += ship->dir[0] * (ship->boosting ? ship->boostAcceleration * dDelta : ship->acceleration * dDelta);
        ship->dy += ship->dir[1] * (ship->boosting ? ship->boostAcceleration * dDelta : ship->acceleration * dDelta);
    }

    if (gb_input_check_state(GB_INPUT_BREAK, GB_INPUT_PRESSED)) {
        ship->dx = ship->dy = 0;
    }

    if (ship->boosting) {
        ship->boosting = !gb_anim_apply(&(ship->sprite->src), delta, ship->anim_boost);
        if (!ship->boosting) {
            ship->thrusting = 1;
            ship->anim_boost->current_frame = 0;
        }
    }

    if (ship->thrusting) {
        gb_anim_apply(&(ship->sprite->src), delta, ship->anim_thrust);
    }

    ship->x += ship->dx * dDelta;
    ship->y += ship->dy * dDelta;

    ship->sprite->dst.x = ship->x - ship->sprite->dst.w * 0.5;
    ship->sprite->dst.y = ship->y - ship->sprite->dst.h * 0.5;
}
