#ifndef ROOM_SYS_H_INCLUDED
#define ROOM_SYS_H_INCLUDED

#include "Room_type.h"

#define ROOM_GRID_WIDTH 2
#define ROOM_GRID_HEIGHT 2

typedef enum {
    SERIALIZE_PLAYER_START = 1,
    SERIALIZE_STATIC_COLLIDERS,
    SERIALIZE_DYNAMIC_ENTITIES,
    SERIALIZE_BACKGROUND_NAME,
    SERIALIZE_POWER_GRID,
    SERIALIZE_END
} ROOM_SERIALIZE_SIGNAL;

Room *roomNew(int gridX, int gridY);
void roomDestroy(Room *room);
int roomLoadBackground(Room * room, char *filename);
void roomUnloadBackground(Room *room);
void roomSerialize(Room *room, char *filepath);
void roomDeserialize(Room *room, char *filepath);

void roomStartActivation(Room *room);
void roomFinishActivation(Room *room);

void roomStartDeactivation(Room *room);
void roomFinishDeactivation(Room *room);

int roomAddStaticCollider(Room *room, CollisionStaticRect *rect);
int roomAddDynamicEntity(Room *room, DynamicEntity *entity);

void roomRefreshPower();

#endif // ROOM_SYS_H_INCLUDED
