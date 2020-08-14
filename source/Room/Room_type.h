#ifndef ROOM_TYPE_H_INCLUDED
#define ROOM_TYPE_H_INCLUDED

#include "../gbGfx/gbGfx_sys.h"

#include "../Collision/Collision_sys.h"
#include "../Collision/CollisionStaticRect_type.h"

#include "../DynamicEntity/DynamicEntity_sys.h"
#include "../DynamicEntity/DynamicEntity_type.h"

#define ROOM_FILENAME_SIZE_MAX 50
#define ROOM_BACKGROUND_FILENAME_SIZE_MAX 50

#define POWER_GRID_GET_WIRING(x) (x & 0x0F)
#define POWER_GRID_GET_STATE(x) (x >> 4)
#define POWER_GRID_CLEAR_WIRING(x) (x &= 0xF0)
#define POWER_GRID_CLEAR_STATE(x, state) (x &= (~state))

#define POWER_GRID_GET_SQUARE_PTR(grid, x, y) (*((grid) + ((x) * GB_GFX_GRID_WIDTH) + (y)))

#define POWER_GRID_ON 16
#define POWER_GRID_BLOCKED 32

typedef enum {
    POWER_GRID_EMPTY,
    POWER_GRID_VERTICAL,
    POWER_GRID_HORIZONTAL,
    POWER_GRID_CORNER_TOP_LEFT,
    POWER_GRID_CORNER_TOP_RIGHT,
    POWER_GRID_CORNER_BOTTOM_RIGHT,
    POWER_GRID_CORNER_BOTTOM_LEFT,
    POWER_GRID_T_NO_BOTTOM,
    POWER_GRID_T_NO_LEFT,
    POWER_GRID_T_NO_TOP,
    POWER_GRID_T_NO_RIGHT,
    POWER_GRID_CROSS,
    POWER_GRID_GENERATOR,
    POWER_GRID_WIRING_NUM
} POWER_GRID_WIRING;

typedef struct {
    char roomFilename[ROOM_FILENAME_SIZE_MAX];
    char backgroundFilename[ROOM_BACKGROUND_FILENAME_SIZE_MAX];

    int gridX;
    int gridY;

    int backgroundTextureId;
    Position backgroundPos;
    Sprite backgroundSprite;

    Position *playerStart;
    uint8_t powerGrid[GB_GFX_GRID_WIDTH][GB_GFX_GRID_HEIGHT];

    int numColliders;
    CollisionStaticRect *staticColliders[COLLISION_MAX_STATIC_COLLIDERS];

    int numEntities;
    DynamicEntity *entities[DYNAMIC_ENTITY_MAX_ENTITIES];
} Room;

#endif // ROOM_TYPE_H_INCLUDED
