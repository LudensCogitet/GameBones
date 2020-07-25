#ifndef ROOM_SYS_H_INCLUDED
#define ROOM_SYS_H_INCLUDED

#include "Room_type.h"

#define ROOM_GRID_WIDTH 2
#define ROOM_GRID_HEIGHT 2

typedef enum {
    SERIALIZE_STATIC_COLLIDERS,
    SERIALIZE_DYNAMIC_ENTITIES,
    SERIALIZE_BACKGROUND_NAME,
    SERIALIZE_END
} ROOM_SERIALIZE_SIGNAL;

Room *roomNew();
void roomDestroy(Room *room);
int roomLoadBackground(Room * room, char *filename);
void roomUnloadBackground(Room *room);
void roomSerialize(Room *room, char *filepath);
void roomDeserialize(Room *room, char *filepath);
void roomActivate(Room *room);
void roomDeactivate(Room *room);

int roomAddStaticCollider(Room *room, CollisionStaticRect *rect);
int roomAddDynamicEntity(Room *room, DynamicEntity *entity);

#endif // ROOM_SYS_H_INCLUDED
