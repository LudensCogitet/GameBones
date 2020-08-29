#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "../../gbUtils/gbUtils.h"

#include "../../gbRenderer/gbRenderer_sys.h"
#include "../../Collision/Collision_sys.h"
#include "../../gbGfx/gbGfx_sys.h"
#include "../../Collision/CollisionDynamicRect_type.h"

#include "../../Position_type.h"
#include "../../gbInput/gbInput_sys.h"
#include "../../gbInput/gbInputState_type.h"
#include "../../gbAnimation/gbAnimation_sys.h"
#include "../../gbAnimation/gbAnimation_type.h"
#include "../../gbAnimation/gbAnimType_type.h"

#include "../../DynamicEntity/DynamicEntityState_union.h"
#include "../../DynamicEntity/DynamicEntity_sys.h"

#include "../../Sprite/Sprite_sys.h"

#include "../../gbTexture/gbTextureName_enum.h"

#include "../../global_state.h"

static int initialized = 0;
static int texture = -1;

DynamicEntity *doorNew(double x, double y, uint32_t state) {
    DynamicEntity *door = dynamicEntityNew(DYNAMIC_ENTITY_TYPE_DOOR);
    door->pos = (Position){x, y};

    int gridX, gridY;
    gbGfxScreenCoordsToGridSquare(x, y, &gridX, &gridY);

    door->dx = gridX;      // Top outlet x
    door->dy = gridY;      // Top outlet y
    door->ax = gridX;      // Bottom outlet x
    door->ay = gridY + 3;  // Bottom outlet Y
    door->state = state;

    int srcX, srcY, srcW, srcH, dstWidth, dstHeight, boxOffsetX, boxOffsetY, boxWidth, boxHeight;

    if (state & DOOR_STATE_HORIZONTAL) {
        srcX = 96;
        srcW = 128;
        srcH = 32;
        width = 128;
        height = 32;

        boxOffsetX = 32;
        boxOffsetY = 11;
        width = 64;
        height = 8;
        if (state & DOOR_STATE_FORCE_FIELD) {
            srcY = 0;
        } else {
            srcY = 64;
        }
    } else {
        srcY = 0;
        srcW = 32;
        srcH = 128;
        width = 32;
        height = 128;

        boxOffsetX = 11;
        boxOffsetY = 32;
        width = 8;
        height = 64;
        if (state & DOOR_STATE_FORCE_FIELD) {
            srcX = 0;
        } else {
            srcX = 64;
        }
    }

    spriteSet(&door->sprite, GB_TEXTURE_NAME_DOOR, srcX, srcY, srcW, srcY, width, height, SPRITE_LAYER_MIDGROUND, 1, 0, SDL_FLIP_NONE);
    collisionDynamicRectSet(&door->boundingBox, door->id, boxOffsetX, boxOffsetY, boxWidth, boxHeight, 1);

    return door;
}

void doorThink(DynamicEntity *door, double delta) {
    if (door->state & DOOR_STATE_OPEN) {
        if (!(currentRoom->powerGrid[(int)door->dx][(int)door->dy] & POWER_GRID_ON) &&
            !(currentRoom->powerGrid[(int)door->ax][(int)door->ay] & POWER_GRID_ON)) {
         door->boundingBox.active = 1;
         door->state = 0;
         door->sprite.src.x = 0;
        }
    } else {
        if ((currentRoom->powerGrid[(int)door->dx][(int)door->dy] & POWER_GRID_ON) ||
            (currentRoom->powerGrid[(int)door->ax][(int)door->ay] & POWER_GRID_ON)) {
         door->boundingBox.active = 0;
         door->state = 1;
         door->sprite.src.x = 32;
        }
    }
}

void doorRespond(DynamicEntity *door, double delta) {

}

void doorInit() {
    if (initialized) return;

    texture = gbTextureLoadNamed(GB_TEXTURE_NAME_DOOR);
    initialized = 1;
}

void doorTeardown() {
    gbTextureUnload(GB_TEXTURE_NAME_DOOR);
    initialized = 0;
}
