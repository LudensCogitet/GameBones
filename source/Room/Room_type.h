#ifndef ROOM_TYPE_H_INCLUDED
#define ROOM_TYPE_H_INCLUDED

#include "../Collision/CollisionStaticRect_type.h"
#include "../DynamicEntity/DynamicEntity_type.h"

typedef struct {
    unsigned int x;
    unsigned int y;

    char roomFilename[50];
    char imageFilename[50];
    CollisionStaticRect *staticColliders;
    DynamicEntity *dynamicEntities;
} Room;

#endif // ROOM_TYPE_H_INCLUDED
