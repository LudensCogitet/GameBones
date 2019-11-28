#include "../headers/entity.h"
#include "../headers/physics.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

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

GbPhysBod *gb_physics_new_body(GbEntity *parent, GB_PHYSICS_COLLIDER_TYPE colliderType, double x, double y, double m, unsigned int dir, float v) {
    if (gb_physics_bodies_cursor >= GB_PHYSICS_MAX_BODIES)
        return 0;

    gb_physics_bodies[gb_physics_bodies_cursor] = (GbPhysBod *)malloc(sizeof(GbPhysBod));

    gb_physics_bodies[gb_physics_bodies_cursor]->parent_entity = parent;

    gb_physics_bodies[gb_physics_bodies_cursor]->x = x;
    gb_physics_bodies[gb_physics_bodies_cursor]->y = y;
    gb_physics_bodies[gb_physics_bodies_cursor]->m = m;
    gb_physics_bodies[gb_physics_bodies_cursor]->dir = gb_physics_directions[dir];

    gb_physics_bodies[gb_physics_bodies_cursor]->collider.circle.collider_type = colliderType;

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

uint8_t gb_physics_detect_collision_circle_circle(GbPhysBod *b1, GbPhysBod *b2) {
    double xDistance = (b1->x - b2->x);
    double yDistance = (b1->y - b2->y);

    return ((xDistance * xDistance) + (yDistance * yDistance)) <
           (b1->collider.circle.radius + b2->collider.circle.radius) * (b1->collider.circle.radius + b2->collider.circle.radius);
}

void gb_physics_detect_collisions() {
    for (unsigned int i = 0; i < gb_physics_bodies_cursor; i++) {
        for (unsigned int j = i; j < gb_physics_bodies_cursor; j++) {
            if (i == j) continue;

            switch (gb_physics_bodies[i]->collider.collider_type) {
                case PHYSICS_COLLIDER_CIRCLE:
                    switch(gb_physics_bodies[j]->collider.collider_type) {
                        case PHYSICS_COLLIDER_CIRCLE:
                            if (gb_physics_detect_collision_circle_circle(gb_physics_bodies[i], gb_physics_bodies[j])) {

                                GbMessage message;

                                message.collision.type = MESSAGE_COLLISION;
                                message.collision.other_collider = gb_physics_bodies[j]->collider;
                                message.collision.x = gb_physics_bodies[j]->x;
                                message.collision.y = gb_physics_bodies[j]->y;
                                message.collision.dx = gb_physics_bodies[j]->dx;
                                message.collision.dy = gb_physics_bodies[j]->dy;
                                message.collision.m = gb_physics_bodies[j]->m;
                                message.collision.other_type = gb_physics_bodies[j]->parent_entity->type;
                                gb_entity_message_send(message, gb_physics_bodies[i]->parent_entity);

                                message.collision.other_collider = gb_physics_bodies[i]->collider;
                                message.collision.x = gb_physics_bodies[i]->x;
                                message.collision.y = gb_physics_bodies[i]->y;
                                message.collision.dx = gb_physics_bodies[i]->dx;
                                message.collision.dy = gb_physics_bodies[i]->dy;
                                message.collision.m = gb_physics_bodies[i]->m;
                                message.collision.other_type = gb_physics_bodies[i]->parent_entity->type;
                                gb_entity_message_send(message, gb_physics_bodies[j]->parent_entity);

                            }
                        break;
                    }
                break;
            }
        }
    }
}

void gb_physics_teardown() {
    for (unsigned int i = 0; i < gb_physics_bodies_cursor; i++) {
        free(gb_physics_bodies[i]);
        gb_physics_bodies[i] = 0;
    }

    gb_physics_bodies_cursor = 0;
}

void gb_physics_resolve_forces(float *dx, float *dy, unsigned int mass, double dx1, double dy1, unsigned int mass1) {
    float ldx = *dx;
    float ldy = *dy;

    *dx = (ldx * (mass - mass1) + (2 * mass1 * dx1)) / (mass + mass1);
    *dy = (ldy * (mass - mass1) + (2 * mass1 * dy1)) / (mass + mass1);

}
