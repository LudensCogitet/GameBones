#include "../../headers/entities/asteroid.h"
#include "../../headers/entity.h"
#include "../../headers/physics.h"
#include <stdlib.h>

Asteroid *asteroid_new(float x, float y, unsigned int dir) {
    Asteroid *asteroid = (Asteroid *)malloc(sizeof(Asteroid));
    GbEntity *entity = gb_entity_add(ENTITY_TYPE_ASTEROID, (void *)asteroid);

    asteroid->body = gb_physics_new_body(entity, x, y, dir, 100);

    asteroid->sprite = gb_gfx_new_sprite(GFX_LAYER_BACKGROUND, GFX_TEXTURE_ASTEROID, 0);

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
}
