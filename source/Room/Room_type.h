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
    int x;
    int y;

    char roomFilename[50];
    char backgroundFilename[50];

    int numColliders;
    CollisionStaticRect *staticColliders[COLLISION_MAX_STATIC_COLLIDERS];

    int numEntities;
    DynamicEntity *entities[DYNAMIC_ENTITY_MAX_ENTITIES];
} Room;

#endif // ROOM_TYPE_H_INCLUDED
