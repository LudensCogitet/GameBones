#ifndef ROOM_H_INCLUDED
#define ROOM_H_INCLUDED

#include "../gbGfx/gbGfx_sys.h"
#include "./roomDirection_enum.h"

#define ROOM_MAX_GEOMETRY 50
#define ROOM_MAX_STATIC_ENTITIES 50
#define ROOM_MAX_DYNAMIC_ENTITIES 25

typedef struct {
    char *filename;
    unsigned int id;
} RoomId;

typedef struct {
    RoomId id;

    gbCollisionStaticRect *geometry[ROOM_MAX_GEOMETRY];
    gbEntity *staticEntities[ROOM_MAX_STATIC_ENTITIES];
    gbEntity *dynamicEntites[ROOM_MAX_DYNAMIC_ENTITIES]

    RoomId neighbors[ROOM_DIRECTION_COUNT];
    uint8_t outlets[ROOM_DIRECTION_COUNT][GB_GFX_GRID_WIDTH]; // @NOTE: This assumes that grid width == grid height

} Room;

#endif // ROOM_H_INCLUDED
