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

GbPhysBod *gb_physics_new_body(GbEntity *parent, GB_PHYSICS_COLLIDER_TYPE colliderType, double x, double y, double m, unsigned int dir, float v, uint8_t solid, uint16_t id) {
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

    gb_physics_bodies[gb_physics_bodies_cursor]->solid = solid;
    gb_physics_bodies[gb_physics_bodies_cursor]->id = id;

    gb_physics_bodies[gb_physics_bodies_cursor]->dispose = 0;

    return gb_physics_bodies[gb_physics_bodies_cursor++];
}

void gb_physics_body_remove(unsigned int entityIndex) {
    free(gb_physics_bodies[entityIndex]);

    if (--gb_physics_bodies_cursor > 0) {
        gb_physics_bodies[entityIndex] = gb_physics_bodies[gb_physics_bodies_cursor];
    }

    gb_physics_bodies[gb_physics_bodies_cursor] = 0;
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

void gb_physics_detect_collisions(double delta) {
    for (unsigned int i = 0; i < gb_physics_bodies_cursor; i++) {
        if (gb_physics_bodies[i]->dispose) {
            gb_physics_body_remove(i);
            if (gb_physics_bodies[i] == 0) break;
        }
        for (unsigned int j = i; j < gb_physics_bodies_cursor; j++) {
            if (i == j) continue;

            if (gb_physics_bodies[j]->dispose) {
                gb_physics_body_remove(j);
                if (gb_physics_bodies[j] == 0) break;
            }

            switch (gb_physics_bodies[i]->collider.collider_type) {
                case PHYSICS_COLLIDER_CIRCLE:
                    switch(gb_physics_bodies[j]->collider.collider_type) {
                        case PHYSICS_COLLIDER_CIRCLE:
                            if (gb_physics_detect_collision_circle_circle(gb_physics_bodies[i], gb_physics_bodies[j])) {

                                GbMessage message;
                                message.type = MESSAGE_COLLISION;
                                message.collision.collision.delta = delta;

                                message.collision.collision.collider1 = gb_physics_bodies[i]->collider;
                                message.collision.collision.x1 = gb_physics_bodies[i]->x;
                                message.collision.collision.y1 = gb_physics_bodies[i]->y;
                                message.collision.collision.dx1 = gb_physics_bodies[i]->dx;
                                message.collision.collision.dy1 = gb_physics_bodies[i]->dy;
                                message.collision.collision.m1 = gb_physics_bodies[i]->m;
                                message.collision.collision.type1 = gb_physics_bodies[i]->parent_entity->type;
                                message.collision.collision.solid1 = gb_physics_bodies[i]->solid;
                                message.collision.collision.id1 = gb_physics_bodies[i]->id;

                                message.collision.collision.collider2 = gb_physics_bodies[j]->collider;
                                message.collision.collision.x2 = gb_physics_bodies[j]->x;
                                message.collision.collision.y2 = gb_physics_bodies[j]->y;
                                message.collision.collision.dx2 = gb_physics_bodies[j]->dx;
                                message.collision.collision.dy2 = gb_physics_bodies[j]->dy;
                                message.collision.collision.m2 = gb_physics_bodies[j]->m;
                                message.collision.collision.type2 = gb_physics_bodies[j]->parent_entity->type;
                                message.collision.collision.solid2 = gb_physics_bodies[j]->solid;
                                message.collision.collision.id2 = gb_physics_bodies[j]->id;

                                gb_entity_message_send(message, gb_physics_bodies[i]->parent_entity);

                                message.collision.collision.collider1 = gb_physics_bodies[j]->collider;
                                message.collision.collision.x1 = gb_physics_bodies[j]->x;
                                message.collision.collision.y1 = gb_physics_bodies[j]->y;
                                message.collision.collision.dx1 = gb_physics_bodies[j]->dx;
                                message.collision.collision.dy1 = gb_physics_bodies[j]->dy;
                                message.collision.collision.m1 = gb_physics_bodies[j]->m;
                                message.collision.collision.type1 = gb_physics_bodies[j]->parent_entity->type;
                                message.collision.collision.solid1 = gb_physics_bodies[j]->solid;
                                message.collision.collision.id1 = gb_physics_bodies[j]->id;

                                message.collision.collision.collider2 = gb_physics_bodies[i]->collider;
                                message.collision.collision.x2 = gb_physics_bodies[i]->x;
                                message.collision.collision.y2 = gb_physics_bodies[i]->y;
                                message.collision.collision.dx2 = gb_physics_bodies[i]->dx;
                                message.collision.collision.dy2 = gb_physics_bodies[i]->dy;
                                message.collision.collision.m2 = gb_physics_bodies[i]->m;
                                message.collision.collision.type2 = gb_physics_bodies[i]->parent_entity->type;
                                message.collision.collision.solid2 = gb_physics_bodies[i]->solid;
                                message.collision.collision.id2 = gb_physics_bodies[i]->id;

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

void gb_physics_resolve_forces(float *dx, float *dy, GbCollision c) {
    *dx = (c.dx1 * (c.m1 - c.m2) + (2 * c.m2 * c.dx2)) / (c.m1 + c.m2);
    *dy = (c.dy1 * (c.m1 - c.m2) + (2 * c.m2 * c.dy2)) / (c.m1 + c.m2);
}
