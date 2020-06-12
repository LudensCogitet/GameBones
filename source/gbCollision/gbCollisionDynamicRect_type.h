#ifndef GB_COLLISION_DYNAMIC_RECT_TYPE_H_INCLUDED
#define GB_COLLISION_DYNAMIC_RECT_TYPE_H_INCLUDED

#include "../gbEntity/gbEntity_type.h"
#include "../gbEntity/gbPosition_type.h"

typedef struct {
    uint8_t dispose;
    uint8_t active;

    gbPosition *pos;
    gbEntity *entity;

    int offsetX;
    int offsetY;

    unsigned int width;
    unsigned int height;
} gbCollisionDynamicRect;

#endif // GB_COLLISION_DYNAMIC_RECT_TYPE_H_INCLUDED
