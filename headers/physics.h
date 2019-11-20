#ifndef PHYSICS_H_INCLUDED
#define PHYSICS_H_INCLUDED

#include <stdint.h>

#define GB_PHYSICS_MAX_BODIES 1000
#define GB_PHYSICS_NUM_DIRS 48

typedef struct GbPhysBod {
    uint8_t dispose;
    double x;
    double y;

    float dx;
    float dy;

    float *dir;

    //GbCollider *collider;
} GbPhysBod;

float gb_physics_directions[GB_PHYSICS_NUM_DIRS][2];

void gb_physics_init();
GbPhysBod *gb_physics_new_body(double x, double y, unsigned int dir, float v);
void gb_physics_body_move(GbPhysBod *body, double delta, float acc);
void gb_physics_teardown();

#endif
