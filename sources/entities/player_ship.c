#include "../../headers/entities/player_ship.h"
#include "../../headers/entity.h"
#include "../../headers/input.h"
#include "../../headers/physics.h"
#include <stdlib.h>

PlayerShip *player_ship_new(float x, float y, unsigned int dir, float acc, float boostAcc) {
    PlayerShip *ship = (PlayerShip *)malloc(sizeof(PlayerShip));

    ship->body = gb_physics_new_body(x, y, dir, 0);

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
    ship->body->dispose = 1;

    free(ship);
}

void player_ship_act(PlayerShip *ship, uint32_t delta) {
    double dDelta = (double)(delta * 0.001);

    float acceleration = 0;

    if (gb_input_check_state(GB_INPUT_ROTATE_LEFT, GB_INPUT_PRESSED)) {
        ship->anim_rotate_ship->direction = 1;
        gb_anim_apply(&(ship->sprite->src), delta, ship->anim_rotate_ship);
        ship->body->dir = gb_physics_directions[ship->anim_rotate_ship->current_frame];
    }

    if (gb_input_check_state(GB_INPUT_ROTATE_RIGHT, GB_INPUT_PRESSED)) {
        ship->anim_rotate_ship->direction = -1;
        gb_anim_apply(&(ship->sprite->src), delta, ship->anim_rotate_ship);
        ship->body->dir = gb_physics_directions[ship->anim_rotate_ship->current_frame];
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
        acceleration = ship->boosting ? ship->boostAcceleration : ship->acceleration;
    }

    if (gb_input_check_state(GB_INPUT_BREAK, GB_INPUT_PRESSED)) {
        ship->body->dx = ship->body->dy = 0;
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

    gb_physics_body_move(ship->body, dDelta, acceleration);
    gb_gfx_sprite_move(ship->body->x, ship->body->y, ship->sprite);
}
