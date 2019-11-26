#ifndef PHYSICS_H_INCLUDED
#define PHYSICS_H_INCLUDED

#include "../headers/entity.h"
#include <stdint.h>

#define GB_PHYSICS_MAX_BODIES 1000
#define GB_PHYSICS_NUM_DIRS 48

typedef enum GB_PHYSICS_COLLIDER_TYPE {
    PHYSICS_COLLIDER_CIRCLE,
    PHYSICS_COLLIDER_NUM_COLLIDER_TYPES
} GB_PHYSICS_COLLIDER_TYPE;

typedef struct GbColliderCircle {
    GB_PHYSICS_COLLIDER_TYPE type;
    double radius;
} GbColliderCircle;

typedef union GbCollider {
    GB_PHYSICS_COLLIDER_TYPE type;
    GbColliderCircle circle;
} GbCollider;

typedef struct GbPhysBod {
    uint8_t dispose;
    double x;
    double y;

    float dx;
    float dy;

    float *dir;

    GbEntity *parent_entity;

    GbCollider collider;
} GbPhysBod;

float gb_physics_directions[GB_PHYSICS_NUM_DIRS][2];

void gb_physics_init();
GbPhysBod *gb_physics_new_body(GbEntity *parent, GB_PHYSICS_COLLIDER_TYPE colliderType, double x, double y, unsigned int dir, float v);
void gb_physics_detect_collisions();
void gb_physics_body_move(GbPhysBod *body, double delta, float acc);
void gb_physics_teardown();

#endif
