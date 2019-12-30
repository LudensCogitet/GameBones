#include "../../headers/entities/player_ship.h"
#include "../../headers/entities/bullet.h"
#include "../../headers/entity.h"
#include "../../headers/input.h"
#include "../../headers/physics.h"
#include "../../headers/renderer.h"
#include "../../headers/sfx.h"
#include <stdlib.h>
#include <stdio.h>

PlayerShip *player_ship_new(float x, float y, unsigned int dir, float acc, float boostAcc) {
    PlayerShip *ship = (PlayerShip *)malloc(sizeof(PlayerShip));
    GbEntity *entity = gb_entity_add(ENTITY_TYPE_PLAYER_SHIP, (void *)ship, ENTITY_PRIORITY_MID);
    ship->body = gb_physics_new_body(entity, PHYSICS_COLLIDER_CIRCLE, x, y, 1, dir, 0);

    ship->body->collider.circle.radius = 15;

    ship->acceleration = acc;
    ship->boostAcceleration = boostAcc;

    ship->boosting = 0;
    ship->thrusting = 0;

    ship->sprite = gb_gfx_new_sprite(
        GFX_LAYER_MIDGROUND,
        GFX_TEXTURE_SHIP,
        0,
        0,
        128,
        128,
        x,
        y,
        GFX_ANCHOR_DEFAULT,
        70,
        70,
        0
    );

    ship->anim_rotate_ship = gb_anim_new_animation(0, 0, 128, 0, 0.02, 48, 1, ANIM_TYPE_LOOP);
    ship->anim_boost = gb_anim_new_animation(0, 128, 0, 128, 0.05, 5, 1, ANIM_TYPE_LOOP);
    ship->anim_thrust = gb_anim_new_animation(0, 128, 0, 128, 0.1, 3, -1, ANIM_TYPE_PINGPONG);

    //gb_gfx_camera_follow(&ship->body->x, &ship->body->y, LOGICAL_SCREEN_WIDTH * 0.25, LOGICAL_SCREEN_HEIGHT * 0.25);
    return ship;
}

void player_ship_destroy(PlayerShip *ship) {
    free(ship->anim_rotate_ship);
    free(ship->anim_boost);
    free(ship->anim_thrust);

    ship->sprite->dispose = 1;
    ship->body->dispose = 1;
    //gb_gfx_camera_unfollow();
    free(ship);
}

void player_ship_act(PlayerShip *ship, double delta) {
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
        gb_sfx_sound_play(SFX_SOUND_SHIP_BOOST, 0);
        ship->boosting = 1;
    }

    if (gb_input_check_state(GB_INPUT_THRUST, GB_INPUT_RELEASED)) {
        ship->boosting = ship->thrusting = ship->anim_thrust->current_frame = ship->anim_boost->current_frame = 0;
        ship->anim_thrust->direction = -1;
        ship->sprite->src.y = 0;
        gb_sfx_sound_stop(ship->sfx_cruise_channel);
    }

    if (gb_input_check_state(GB_INPUT_THRUST, GB_INPUT_PRESSED)) {
        acceleration = ship->boosting ? ship->boostAcceleration : ship->acceleration;
    }

    if (gb_input_check_state(GB_INPUT_FIRE, GB_INPUT_JUST_PRESSED)) {
        bullet_new(
            ship->body->x + (ship->body->dir[0] * 30),
            ship->body->y + (ship->body->dir[1] * 30),
            ship->body->dx + (ship->body->dir[0] * 1000),
            ship->body->dy + (ship->body->dir[1] * 1000),
            0.5
        );
    }

    if (ship->boosting) {
        ship->boosting = !gb_anim_apply(&(ship->sprite->src), delta, ship->anim_boost);
        if (!ship->boosting) {
            ship->sfx_cruise_channel = gb_sfx_sound_play(SFX_SOUND_SHIP_CRUISE, -1);
            ship->thrusting = 1;
            ship->anim_boost->current_frame = 0;
        }
    }

    if (ship->thrusting) {
        gb_anim_apply(&(ship->sprite->src), delta, ship->anim_thrust);
    }

    gb_physics_body_move(ship->body, delta, acceleration);
    gb_gfx_sprite_move(ship->body->x, ship->body->y, ship->sprite);
}

void player_ship_handle_messages(PlayerShip *ship, GbMessage *messages, unsigned int numMessages) {
    for (unsigned int i = 0; i < numMessages; i++) {
        switch (messages[i].type) {
            case MESSAGE_COLLISION:
                gb_sfx_sound_play(SFX_SOUND_SHIP_CRASH, 0);
                gb_physics_resolve_forces(&ship->body->dx, &ship->body->dy, messages[i].collision.collision);
            break;
        }
    }
}
