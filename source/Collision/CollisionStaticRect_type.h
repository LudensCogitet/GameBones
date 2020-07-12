#ifndef COLLISION_STATIC_RECT_TYPE_H_INCLUDED
#define COLLISION_STATIC_RECT_TYPE_H_INCLUDED

#include <stdint.h>

typedef struct {
    uint8_t active;
    int x1;
    int y1;
    int x2;
    int y2;
} CollisionStaticRect;

#endif // COLLISION_STATIC_RECT_TYPE_H_INCLUDED
