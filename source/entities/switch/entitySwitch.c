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

DynamicEntity *switchNew(double x, double y) {
    DynamicEntity *switchEntity = dynamicEntityNew(DYNAMIC_ENTITY_TYPE_SWITCH);
    switchEntity->pos = (Position){x, y};

    int gridX, gridY;
    gbGfxScreenCoordsToGridSquare(x, y, &gridX, &gridY);

    switchEntity->dx = gridX;       // Power grid target x
    switchEntity->dy = gridY + 1;   // Power grid target Y

    switchEntity->ax = 0;       // on / off flag
    switchEntity->ay = 1;       // first tick flag

    spriteSet(&switchEntity->sprite, GB_TEXTURE_NAME_SWITCH, 0, 0, 32, 32, 32, 32, SPRITE_LAYER_BACKMID, 1, 0, SDL_FLIP_NONE);
    collisionDynamicRectSet(&switchEntity->boundingBox, switchEntity->id, 0, 0, 32, 32, 0);

    return switchEntity;
}

void switchThink(DynamicEntity *switchEntity, double delta) {
    if (switchEntity->ay) {
        switchEntity->ay = 0;
        currentRoom->powerGrid[(int)switchEntity->dx][(int)switchEntity->dy] |= POWER_GRID_BLOCKED;
    }

    if (gbInputCheckState(GB_INPUT_INTERACT, GB_INPUT_RELEASED)) {
        if (collisionCheckPlayer(switchEntity)) {
            switchEntity->ax = !switchEntity->ax;

            switchEntity->sprite.src.x = switchEntity->ax * 32;

            if (!switchEntity->ax) {
                currentRoom->powerGrid[(int)switchEntity->dx][(int)switchEntity->dy] |= POWER_GRID_BLOCKED;
            } else {
                POWER_GRID_CLEAR_STATE(currentRoom->powerGrid[(int)switchEntity->dx][(int)switchEntity->dy], POWER_GRID_BLOCKED);
            }

            roomRefreshPower();
        }
    }
}

void switchRespond(DynamicEntity *switchEntity, double delta) {

}

void switchInit() {
    if (initialized) return;

    texture = gbTextureLoadNamed(GB_TEXTURE_NAME_SWITCH);
    initialized = 1;
}

void switchTeardown() {
    gbTextureUnload(GB_TEXTURE_NAME_SWITCH);
    initialized = 0;
}
