#ifndef ROOM_TYPE_H_INCLUDED
#define ROOM_TYPE_H_INCLUDED

#include "../Collision/Collision_sys.h"
#include "../Collision/CollisionStaticRect_type.h"

#include "../DynamicEntity/DynamicEntity_sys.h"
#include "../DynamicEntity/DynamicEntity_type.h"

typedef struct {
    Position pos;
    uint32_t type;
} DynamicEntityData;

typedef struct {
    uint16_t x;
    uint16_t y;

    char roomFilename[50];
    char backgroundFilename[50];
    CollisionStaticRect staticColliders[COLLISION_MAX_STATIC_COLLIDERS];
    DynamicEntityData entities[DYNAMIC_ENTITY_MAX_ENTITIES];
} Room;

#endif // ROOM_TYPE_H_INCLUDED
