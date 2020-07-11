#ifndef COLLISION_DYNAMIC_RECT_TYPE_H_INCLUDED
#define COLLISION_DYNAMIC_RECT_TYPE_H_INCLUDED

#include "../Position_type.h"

typedef struct {
    Position *pos;

    unsigned int entityId;
    uint8_t active;

    int offsetX;
    int offsetY;

    unsigned int width;
    unsigned int height;
} CollisionDynamicRect;

#endif // COLLISION_DYNAMIC_RECT_TYPE_H_INCLUDED
