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

#include "../../DynamicEntity/DynamicEntity_sys.h"

#include "../../Sprite/Sprite_sys.h"

#include "../../gbTexture/gbTextureName_enum.h"

#include "entityMoveRoomPanel.h"

#include "../../global_state.h"

static int initialized = 0;
static int texture = -1;

extern int validRoomIndex(int dx, int dy);
extern void handleRoomMove(int dx, int dy);

DynamicEntity *moveRoomPanelNew(double x, double y, uint32_t state) {
    DynamicEntity *panel = dynamicEntityNew(DYNAMIC_ENTITY_TYPE_MOVE_ROOM_PANEL);
    panel->pos = (Position){x, y};

    panel->dx = 0;      // Change room this dir X
    panel->dy = 0;      // Change room this dir Y
    panel->ax = 0;      // Player in bounding box flag
    panel->ay = 0;
    panel->state = MOVE_ROOM_PANEL_STATE_EMPTY;

    spriteSet(&panel->sprite, GB_TEXTURE_NAME_MOVE_ROOM_PANEL, 0, 0, 32, 64, 32, 64, SPRITE_LAYER_BACKMID, 1, 0, SDL_FLIP_NONE);
    collisionDynamicRectSet(&panel->boundingBox, panel->id, 0, 0, 32, 64, 0);

    return panel;
}

void updateDirection(DynamicEntity *panel, uint32_t newState) {
    int dx = 0;
    int dy = 0;
    int validIndex = newState;

    if (newState) {
        switch (newState) {
            case MOVE_ROOM_PANEL_STATE_DOWN:
                dy = 1;
                break;
            case MOVE_ROOM_PANEL_STATE_LEFT:
                dx = -1;
                break;
            case MOVE_ROOM_PANEL_STATE_RIGHT:
                dx = 1;
                break;
            case MOVE_ROOM_PANEL_STATE_UP:
                dy = -1;
        }

        panel->state = newState;
    } else {
        int oldState = panel->state;
        do {
            panel->state++;

            if (panel->state == MOVE_ROOM_PANEL_STATE_NO_STATE)
                panel->state = 1;

            switch (panel->state) {
                case MOVE_ROOM_PANEL_STATE_DOWN:
                    if (validIndex = validRoomIndex(0, 1))
                        dy = 1;
                    break;
                case MOVE_ROOM_PANEL_STATE_LEFT:
                    if (validIndex = validRoomIndex(-1, 0))
                        dx = -1;
                    break;
                case MOVE_ROOM_PANEL_STATE_RIGHT:
                    if (validIndex = validRoomIndex(1, 0))
                        dx = 1;
                    break;
                case MOVE_ROOM_PANEL_STATE_UP:
                    if (validIndex = validRoomIndex(0, -1))
                        dy = -1;
            }
        } while (!validIndex && panel->state != oldState);
    }

    panel->sprite.src.x = panel->sprite.src.w * panel->state;
    panel->dx = dx;
    panel->dy = dy;
}

void moveRoomPanelThink(DynamicEntity *panel, double delta) {
    if (gbInputCheckState(GB_INPUT_SELECT, GB_INPUT_RELEASED)) {
        if (collisionCheckPlayer(panel))
            updateDirection(panel, 0);
    } else if (gbInputCheckState(GB_INPUT_INTERACT, GB_INPUT_RELEASED)) {
        if (collisionCheckPlayer(panel)) {
            handleRoomMove(panel->dx, panel->dy);
            switch (panel->state) {
                case MOVE_ROOM_PANEL_STATE_DOWN:
                    updateDirection(panel, MOVE_ROOM_PANEL_STATE_UP);
                    break;
                case MOVE_ROOM_PANEL_STATE_LEFT:
                    updateDirection(panel, MOVE_ROOM_PANEL_STATE_RIGHT);
                    break;
                case MOVE_ROOM_PANEL_STATE_RIGHT:
                    updateDirection(panel, MOVE_ROOM_PANEL_STATE_LEFT);
                    break;
                case MOVE_ROOM_PANEL_STATE_UP:
                    updateDirection(panel, MOVE_ROOM_PANEL_STATE_DOWN);
            }
        }
    }
}

void moveRoomPanelRespond(DynamicEntity *panel, double delta) {

}

void moveRoomPanelInit() {
    if (initialized) return;

    texture = gbTextureLoadNamed(GB_TEXTURE_NAME_MOVE_ROOM_PANEL);
    initialized = 1;
}

void moveRoomPanelTeardown() {
    gbTextureUnload(GB_TEXTURE_NAME_MOVE_ROOM_PANEL);
    initialized = 0;
}
