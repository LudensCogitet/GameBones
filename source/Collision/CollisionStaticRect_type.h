#ifndef COLLISION_STATIC_RECT_TYPE_H_INCLUDED
#define COLLISION_STATIC_RECT_TYPE_H_INCLUDED

#include <stdint.h>

typedef struct {
    uint8_t active;
    int32_t x1;
    int32_t y1;
    int32_t x2;
    int32_t y2;
} CollisionStaticRect;

#endif // COLLISION_STATIC_RECT_TYPE_H_INCLUDED
