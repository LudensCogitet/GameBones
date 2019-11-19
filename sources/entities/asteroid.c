#include "../../headers/entities/asteroid.h"
#include "../../headers/entity.h"

static float directions[48][2] = {
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

Asteroid *asteroid_new(float x, float y, unsigned int dir) {
    Asteroid *asteroid = (Asteroid *)malloc(sizeof(Asteroid));

    asteroid->x = x;
    asteroid->y = y;

    asteroid->dir = directions[dir];

    asteroid->dx = asteroid->dir[0] * 100;
    asteroid->dy = asteroid->dir[1] * 100;
    asteroid->sprite = gb_gfx_new_sprite(GFX_LAYER_BACKGROUND, GFX_TEXTURE_ASTEROID, 0);

    asteroid->sprite->src.x = 0;
    asteroid->sprite->src.y = 0;
    asteroid->sprite->src.w = 128;
    asteroid->sprite->src.h = 128;

    asteroid->sprite->dst.x = x - asteroid->sprite->dst.w * 0.5;
    asteroid->sprite->dst.y = y - asteroid->sprite->dst.h * 0.5;
    asteroid->sprite->dst.w = 110;
    asteroid->sprite->dst.h = 110;

    asteroid->anim_rotate = gb_anim_new_animation(0, 0, 128, 0, 50, 23, 1, ANIM_TYPE_LOOP);

    gb_entity_add(ENTITY_TYPE_ASTEROID, (void *)asteroid);

    return asteroid;
}

void asteroid_destroy(Asteroid *asteroid) {
    free(asteroid->anim_rotate);

    asteroid->sprite->dispose = 1;

    free(asteroid);
}

void asteroid_act(Asteroid *asteroid, uint32_t delta) {
    double dDelta = (double)(delta * 0.001);

    gb_anim_apply(&(asteroid->sprite->src), delta, asteroid->anim_rotate);

    asteroid->x += asteroid->dx * dDelta;
    asteroid->y += asteroid->dy * dDelta;

    asteroid->sprite->dst.x = asteroid->x - asteroid->sprite->dst.w * 0.5;
    asteroid->sprite->dst.y = asteroid->y - asteroid->sprite->dst.h * 0.5;
}
