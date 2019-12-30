#include "../../headers/entities/bullet.h"
#include "../../headers/entity.h"
#include "../../headers/input.h"
#include "../../headers/physics.h"
#include "../../headers/renderer.h"
#include "../../headers/sfx.h";
#include <stdlib.h>
#include <stdio.h>

Bullet *bullet_new(float x, float y, double dx, double dy, double lifespan) {
    Bullet *bullet = (Bullet *)malloc(sizeof(Bullet));
    bullet->self = gb_entity_add(ENTITY_TYPE_BULLET, (void *)bullet, ENTITY_PRIORITY_MID);

    bullet->body = gb_physics_new_body(bullet->self, PHYSICS_COLLIDER_CIRCLE, x, y, 0.2, 0, 0);

    bullet->body->collider.circle.radius = 5;

    bullet->body->dx = dx;
    bullet->body->dy = dy;

    bullet->accumulator = 0;
    bullet->lifespan = lifespan;

    bullet->sprite = gb_gfx_new_sprite(
        GFX_LAYER_MIDGROUND,
        GFX_TEXTURE_BULLET,
        0,
        0,
        32,
        32,
        x,
        y,
        GFX_ANCHOR_DEFAULT,
        10,
        10,
        0
    );

    return bullet;
}

void bullet_destroy(Bullet *bullet) {
    bullet->sprite->dispose = 1;
    bullet->body->dispose = 1;

    free(bullet);

}

void bullet_act(Bullet *bullet, double delta) {
    gb_physics_body_move(bullet->body, delta, 0);
    gb_gfx_sprite_move(bullet->body->x, bullet->body->y, bullet->sprite);

    bullet->accumulator += delta;

    if (bullet->accumulator >= bullet->lifespan) {
        bullet->self->dispose = 1;
    }
}

void bullet_handle_messages(Bullet *bullet, GbMessage *messages, unsigned int numMessages) {
    for (unsigned int i = 0; i < numMessages; i++) {
        switch (messages[i].type) {
            case MESSAGE_COLLISION:
                bullet->self->dispose = 1;
            break;
        }
    }
}
