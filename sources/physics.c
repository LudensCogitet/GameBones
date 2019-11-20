#include "../headers/physics.h"
#include <stdlib.h>

float gb_physics_directions[GB_PHYSICS_NUM_DIRS][2] = {
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

static GbPhysBod *gb_physics_bodies[GB_PHYSICS_MAX_BODIES];
static uint32_t gb_physics_bodies_cursor;

void gb_physics_init() {
    for (unsigned int i = 0; i < GB_PHYSICS_MAX_BODIES; i++) {
        gb_physics_bodies[i] = 0;
    }

    gb_physics_bodies_cursor = 0;
}

GbPhysBod *gb_physics_new_body(double x, double y, unsigned int dir, float v) {
    if (gb_physics_bodies_cursor >= GB_PHYSICS_MAX_BODIES)
        return 0;

    gb_physics_bodies[gb_physics_bodies_cursor] = (GbPhysBod *)malloc(sizeof(GbPhysBod));

    gb_physics_bodies[gb_physics_bodies_cursor]->x = x;
    gb_physics_bodies[gb_physics_bodies_cursor]->y = y;
    gb_physics_bodies[gb_physics_bodies_cursor]->dir = gb_physics_directions[dir];

    gb_physics_bodies[gb_physics_bodies_cursor]->dx = gb_physics_bodies[gb_physics_bodies_cursor]->dir[0] * v;
    gb_physics_bodies[gb_physics_bodies_cursor]->dy = gb_physics_bodies[gb_physics_bodies_cursor]->dir[1] * v;

    gb_physics_bodies[gb_physics_bodies_cursor]->dispose = 0;

    return gb_physics_bodies[gb_physics_bodies_cursor++];
}

void gb_physics_body_move(GbPhysBod *body, double delta, float acc) {
    if (acc) {
        body->dx += (body->dir[0] * acc) * delta;
        body->dy += (body->dir[1] * acc) * delta;
    }

    body->x += body->dx * delta;
    body->y += body->dy * delta;
}

void gb_physics_teardown() {
    for (unsigned int i = 0; i < gb_physics_bodies_cursor; i++) {
        free(gb_physics_bodies[i]);
        gb_physics_bodies[i] = 0;
    }

    gb_physics_bodies_cursor = 0;
}
