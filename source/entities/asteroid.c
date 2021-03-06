#include "../../headers/entities/asteroid.h"
#include "../../headers/input.h"
#include "../../headers/entity.h"
#include "../../headers/physics.h"
#include <stdlib.h>

Asteroid *asteroid_new(float x, float y, unsigned int dir, float v) {
    Asteroid *asteroid = (Asteroid *)malloc(sizeof(Asteroid));
    GbEntity *entity = gb_entity_add(ENTITY_TYPE_ASTEROID, (void *)asteroid, ENTITY_PRIORITY_MID);

    asteroid->body = gb_physics_new_body(entity, PHYSICS_COLLIDER_CIRCLE, x, y, 3, dir, v, 1, 0);
    asteroid->body->collider.circle.radius = 20;

    asteroid->sprite = gb_gfx_new_sprite(
        GFX_LAYER_MIDGROUND,
        GFX_TEXTURE_ASTEROID,
        0,
        0,
        128,
        128,
        x,
        y,
        GFX_ANCHOR_DEFAULT,
        90,
        90,
        0
    );

    asteroid->sprite->src.x = 0;
    asteroid->sprite->src.y = 0;
    asteroid->sprite->src.w = 128;
    asteroid->sprite->src.h = 128;

    asteroid->sprite->dst.x = x - asteroid->sprite->dst.w * 0.5;
    asteroid->sprite->dst.y = y - asteroid->sprite->dst.h * 0.5;
    asteroid->sprite->dst.w = 110;
    asteroid->sprite->dst.h = 110;

    asteroid->anim_rotate = gb_anim_new_animation(0, 0, 128, 0, 0.08, 23, 1, ANIM_TYPE_LOOP);

    return asteroid;
}

void asteroid_destroy(Asteroid *asteroid) {
    free(asteroid->anim_rotate);

    asteroid->sprite->dispose = 1;
    asteroid->body->dispose = 1;

    free(asteroid);
}

void asteroid_act(Asteroid *asteroid, double delta) {
    gb_anim_apply(&(asteroid->sprite->src), delta, asteroid->anim_rotate);

    gb_physics_body_move(asteroid->body, delta, 0);
    gb_gfx_sprite_move(asteroid->body->x, asteroid->body->y, asteroid->sprite);

    if (gb_input_check_state(GB_INPUT_SELECT, GB_INPUT_JUST_PRESSED)) {
        int x, y;
        gb_input_get_mouse_pos_world(&x, &y);
        asteroid_new(x, y, 0, 0);
        gb_input_clear_state(GB_INPUT_SELECT);
    }
}


void asteroid_handle_messages(Asteroid *asteroid, GbMessage *messages, unsigned int numMessages) {
    for (unsigned int i = 0; i < numMessages; i++) {
        switch (messages[i].type) {
            case MESSAGE_COLLISION:
                if (!messages[i].collision.collision.solid2) break;

                gb_sfx_sound_play(SFX_SOUND_ASTEROID_HIT, 0);
                gb_physics_resolve_forces(&asteroid->body->dx, &asteroid->body->dy, messages[i].collision.collision);
            break;
        }
    }
}
