#ifndef ROOM_TYPE_H_INCLUDED
#define ROOM_TYPE_H_INCLUDED

#include "../Collision/Collision_sys.h"
#include "../Collision/CollisionStaticRect_type.h"

#include "../DynamicEntity/DynamicEntity_sys.h"
#include "../DynamicEntity/DynamicEntity_type.h"

#define ROOM_FILENAME_SIZE_MAX 50
#define ROOM_BACKGROUND_FILENAME_SIZE_MAX 50

typedef struct {
    char roomFilename[ROOM_FILENAME_SIZE_MAX];
    char backgroundFilename[ROOM_BACKGROUND_FILENAME_SIZE_MAX];

    int backgroundTextureId;
    Position backgroundPos;
    Sprite backgroundSprite;

    Position *playerStart;

    int numColliders;
    CollisionStaticRect *staticColliders[COLLISION_MAX_STATIC_COLLIDERS];

    int numEntities;
    DynamicEntity *entities[DYNAMIC_ENTITY_MAX_ENTITIES];
} Room;

#endif // ROOM_TYPE_H_INCLUDED
