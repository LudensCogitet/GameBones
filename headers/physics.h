#ifndef PHYSICS_H_INCLUDED
#define PHYSICS_H_INCLUDED

#include <stdint.h>
#include "../headers/enums.h"

#define PI 3.141592654

#define GB_PHYSICS_MAX_BODIES 1000
#define GB_PHYSICS_NUM_DIRS 48

typedef struct GbColliderCircle {
    GB_PHYSICS_COLLIDER_TYPE collider_type;
    double radius;
} GbColliderCircle;

typedef union GbCollider {
    GB_PHYSICS_COLLIDER_TYPE collider_type;
    GbColliderCircle circle;
} GbCollider;

typedef struct GbCollision {
    double x1;
    double y1;
    float dx1;
    float dy1;
    double m1;
    GB_ENTITY_TYPE type1;
    GbCollider collider1;

    double x2;
    double y2;
    float dx2;
    float dy2;
    double m2;
    GB_ENTITY_TYPE type2;
    GbCollider collider2;

    double delta;
} GbCollision;

typedef struct GbEntity GbEntity;
typedef struct GbPhysBod {
    uint8_t dispose;
    double x;
    double y;

    float dx;
    float dy;

    float *dir;

    double m;

    GbEntity *parent_entity;

    GbCollider collider;
} GbPhysBod;

float gb_physics_directions[GB_PHYSICS_NUM_DIRS][2];

void gb_physics_init();
GbPhysBod *gb_physics_new_body(GbEntity *parent, GB_PHYSICS_COLLIDER_TYPE colliderType, double x, double y, double m, unsigned int dir, float v);
void gb_physics_detect_collisions(double delta);
void gb_physics_body_move(GbPhysBod *body, double delta, float acc);
void gb_physics_resolve_forces(float *dx, float *dy, GbCollision c);
void gb_physics_teardown();

#endif
