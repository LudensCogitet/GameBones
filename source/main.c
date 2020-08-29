#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include <SDL2/SDL.h>

#include "./gbRenderer/gbRenderer_sys.h"
#include "./gbInput/gbInput_sys.h"
#include "./gbGfx/gbGfx_sys.h"
#include "./gbAnimation/gbAnimation_sys.h"
#include "./Collision/Collision_sys.h"

#include "./gbTexture/gbTexture_sys.h"

#include "./gbInput/gbInput_type.h"
#include "./Sprite/Sprite_type.h"
#include "./gbAnimation/gbAnimation_type.h"
#include "./gbAnimation/gbAnimType_type.h"
#include "./DynamicEntity/DynamicEntity_sys.h"
#include "./editor/editor.h"

#include "./gbGfx/gbFont.h"
#include "./gbGfx/gbColor.h"
#include "./gbGfx/gbText/gbText_type.h"

#include "Room/Room_type.h"
#include "Room/Room_sys.h"

#include "Sprite/Sprite_sys.h"

#include "entities/Guy/entityGuy.h"
#include "entities/MoveRoomPanel/entityMoveRoomPanel.h"
#include "global_state.h"

static enum {
    GRID_BOUND_LEFT,
    GRID_BOUND_RIGHT,
    GRID_BOUND_TOP,
    GRID_BOUND_BOTTOM
};

static CollisionStaticRect bounds[4];

unsigned int mainCabelsTexture;

int validRoomIndex(int dx, int dy) {
    int newRoomX = activeRoomX + dx;
    int newRoomY = activeRoomY + dy;

    return newRoomX >= 0 && newRoomX <= MAIN_ROOM_GRID_WIDTH - 1 &&
           newRoomY >= 0 && newRoomY <= MAIN_ROOM_GRID_HEIGHT - 1;
}

void setPlayerPosition(double x, double y) {
    if (!mainPlayer) {
        mainPlayer = guyNew(x, y, 0);
        dynamicEntityRegister(mainPlayer);
    } else {
        mainPlayer->pos.x = x;
        mainPlayer->pos.y = y;
    }
}

void setRoomBounds() {
    bounds[GRID_BOUND_RIGHT].active = !validRoomIndex(1, 0);
    bounds[GRID_BOUND_LEFT].active = !validRoomIndex(-1, 0);
    bounds[GRID_BOUND_TOP].active = !validRoomIndex(0, -1);
    bounds[GRID_BOUND_BOTTOM].active = !validRoomIndex(0, 1);
}

void gameInit() {
    int x1, y1, x2, y2;

    x1 = GB_GFX_GRID_OFFSET_X - GB_GFX_GRID_SIZE;
    y1 = GB_GFX_GRID_OFFSET_Y - GB_GFX_GRID_SIZE;
    x2 = GB_GFX_GRID_OFFSET_X;
    y2 = GB_GFX_GRID_OFFSET_Y + (GB_GFX_GRID_HEIGHT * GB_GFX_GRID_SIZE) + GB_GFX_GRID_SIZE;
    collisionStaticRectSet(&bounds[GRID_BOUND_LEFT], x1, y1, x2, y2, 1);

    x1 = GB_GFX_GRID_OFFSET_X + (GB_GFX_GRID_WIDTH * GB_GFX_GRID_SIZE);
    // y1 is same as above
    x2 = x1 + GB_GFX_GRID_SIZE;
    // y2 is same as above
    collisionStaticRectSet(&bounds[GRID_BOUND_RIGHT], x1, y1, x2, y2, 1);

    x1 = GB_GFX_GRID_OFFSET_X - GB_GFX_GRID_SIZE;
    // y1 is the same as above
    x2 = GB_GFX_GRID_OFFSET_X + (GB_GFX_GRID_WIDTH * GB_GFX_GRID_SIZE) + GB_GFX_GRID_SIZE;
    y2 = y1 + GB_GFX_GRID_SIZE;
    collisionStaticRectSet(&bounds[GRID_BOUND_TOP], x1, y1, x2, y2, 1);

    // x1 is same as above
    y1 = GB_GFX_GRID_OFFSET_Y + (GB_GFX_GRID_HEIGHT * GB_GFX_GRID_SIZE);
    // x2 is same as above
    y2 = y1 + GB_GFX_GRID_SIZE;
    collisionStaticRectSet(&bounds[GRID_BOUND_BOTTOM], x1, y1, x2, y2, 1);

    for (unsigned int i = 0; i < 8; i++) {
        collisionStaticRectRegister(&bounds[i]);
    }

    for (unsigned int x = 0; x < MAIN_ROOM_GRID_WIDTH; x++) {
        for (unsigned int y = 0; y < MAIN_ROOM_GRID_HEIGHT; y++) {
            rooms[x][y] = roomNew(x, y);
        }
    }
    roomDeserialize(rooms[1][0], "./room1.rm");
    roomDeserialize(rooms[1][1], "./room1.rm");
    roomDeserialize(rooms[0][1], "./room1.rm");
    roomDeserialize(rooms[0][0], "./room1.rm");

    activeRoomX = 1;
    activeRoomY = 0;

    mainPowerGridWidth = GB_GFX_GRID_WIDTH * MAIN_ROOM_GRID_WIDTH;
    mainPowerGridHeight = GB_GFX_GRID_HEIGHT * MAIN_ROOM_GRID_HEIGHT;

    roomStartActivation(rooms[activeRoomX][activeRoomY]);
    roomFinishActivation(rooms[activeRoomX][activeRoomY]);

    setRoomBounds();

    roomRefreshPower();
}

int handleRoomChange(double delta) {
    static uint8_t roomTransition = 0;
    static int oldRoomX = -1;
    static int oldRoomY = -1;
    static int roomDx = 0;
    static int roomDy = 0;

    if (!roomTransition) {
        double pLeftExtremity = mainPlayer->pos.x + mainPlayer->boundingBox.offsetX;
        double pRightExtremity = mainPlayer->pos.x + mainPlayer->boundingBox.offsetX + mainPlayer->boundingBox.width;

        double pTopExtremity = mainPlayer->pos.y + mainPlayer->boundingBox.offsetY;
        double pBottomExtremity = mainPlayer->pos.y + mainPlayer->boundingBox.offsetY + mainPlayer->boundingBox.height;

        // Player has exited grid to the left
        if (pRightExtremity < GB_GFX_GRID_OFFSET_X && activeRoomX > 0) {
            oldRoomX = activeRoomX;
            oldRoomY = activeRoomY;

            activeRoomX--;

            mainPlayer->pos.x += GB_GFX_GRID_SIZE * GB_GFX_GRID_WIDTH;
            roomDx = (GB_GFX_GRID_WIDTH * GB_GFX_GRID_SIZE);
        }

        // Player has exited grid to the right
        if (pLeftExtremity > (GB_GFX_GRID_OFFSET_X + (GB_GFX_GRID_SIZE * GB_GFX_GRID_WIDTH)) && activeRoomX < MAIN_ROOM_GRID_WIDTH - 1) {
            oldRoomX = activeRoomX;
            oldRoomY = activeRoomY;

            activeRoomX++;

            mainPlayer->pos.x = GB_GFX_GRID_OFFSET_X;
            roomDx = -(GB_GFX_GRID_WIDTH * GB_GFX_GRID_SIZE);
        }

        // Player has exited grid by the top
        if (pBottomExtremity < GB_GFX_GRID_OFFSET_Y && activeRoomY > 0) {
            oldRoomX = activeRoomX;
            oldRoomY = activeRoomY;

            activeRoomY--;

            mainPlayer->pos.y += GB_GFX_GRID_SIZE * GB_GFX_GRID_HEIGHT;
            roomDy = (GB_GFX_GRID_HEIGHT * GB_GFX_GRID_SIZE);
        }

        // Player has exited grid through the bottom
        if (pTopExtremity > (GB_GFX_GRID_OFFSET_Y + (GB_GFX_GRID_SIZE * GB_GFX_GRID_HEIGHT)) && activeRoomY < MAIN_ROOM_GRID_HEIGHT - 1) {
            oldRoomX = activeRoomX;
            oldRoomY = activeRoomY;

            activeRoomY++;

            mainPlayer->pos.y = GB_GFX_GRID_OFFSET_Y;
            roomDy = -(GB_GFX_GRID_HEIGHT * GB_GFX_GRID_SIZE);
        }

        if (oldRoomX > -1 && oldRoomY > -1) {
            mainPlayer->sprite.active = 0;
            roomStartDeactivation(rooms[oldRoomX][oldRoomY]);
            roomStartActivation(rooms[activeRoomX][activeRoomY]);
            rooms[activeRoomX][activeRoomY]->backgroundPos.x -= roomDx;
            rooms[activeRoomX][activeRoomY]->backgroundPos.y -= roomDy;
            roomTransition = 1;
        }
    } else {
        uint8_t done = 0;
        double newX = rooms[activeRoomX][activeRoomY]->backgroundPos.x;
        double newY = rooms[activeRoomX][activeRoomY]->backgroundPos.y;

        double dx = 0;
        double dy = 0;
        if (newX < GB_GFX_GRID_OFFSET_X) {
            dx = roomDx * delta;
            rooms[activeRoomX][activeRoomY]->backgroundPos.x += dx;
            rooms[oldRoomX][oldRoomY]->backgroundPos.x += dx;

            done = rooms[activeRoomX][activeRoomY]->backgroundPos.x >= GB_GFX_GRID_OFFSET_X;
        } else if (newX > GB_GFX_GRID_OFFSET_X) {
            dx = roomDx * delta;
            rooms[activeRoomX][activeRoomY]->backgroundPos.x += dx;
            rooms[oldRoomX][oldRoomY]->backgroundPos.x += dx;

            done = rooms[activeRoomX][activeRoomY]->backgroundPos.x <= GB_GFX_GRID_OFFSET_X;
        } else if (newY < GB_GFX_GRID_OFFSET_Y) {
            dy = roomDy * delta;
            rooms[activeRoomX][activeRoomY]->backgroundPos.y += dy;
            rooms[oldRoomX][oldRoomY]->backgroundPos.y += dy;

            done = rooms[activeRoomX][activeRoomY]->backgroundPos.y >= GB_GFX_GRID_OFFSET_Y;
        } else if (newY > GB_GFX_GRID_OFFSET_Y) {
            dy = roomDy * delta;
            rooms[activeRoomX][activeRoomY]->backgroundPos.y += dy;
            rooms[oldRoomX][oldRoomY]->backgroundPos.y += dy;

            done = rooms[activeRoomX][activeRoomY]->backgroundPos.y <= GB_GFX_GRID_OFFSET_Y;
        }

        if (done) {
            mainPlayer->sprite.active = 1;
            roomFinishDeactivation(rooms[oldRoomX][oldRoomY]);
            rooms[oldRoomX][oldRoomY]->backgroundPos.x = GB_GFX_GRID_OFFSET_X;
            rooms[oldRoomX][oldRoomY]->backgroundPos.y = GB_GFX_GRID_OFFSET_Y;
            oldRoomX = -1;
            oldRoomY = -1;
            roomDx = 0;
            roomDy = 0;

            roomFinishActivation(rooms[activeRoomX][activeRoomY]);
            roomTransition = 0;
            setRoomBounds();
        }
    }

    return roomTransition;
}

void handleRoomMove(int dx, int dy) {
    if (!validRoomIndex(dx, dy)) return;

    int oldRoomX = activeRoomX;
    int oldRoomY = activeRoomY;
    activeRoomX = activeRoomX + dx;
    activeRoomY = activeRoomY + dy;

    Room *swap = rooms[activeRoomX][activeRoomY];
    rooms[activeRoomX][activeRoomY] = rooms[oldRoomX][oldRoomY];
    rooms[oldRoomX][oldRoomY] = swap;

    rooms[activeRoomX][activeRoomY]->gridX = activeRoomX;
    rooms[activeRoomX][activeRoomY]->gridY = activeRoomY;
    rooms[oldRoomX][oldRoomY]->gridX = oldRoomX;
    rooms[oldRoomX][oldRoomY]->gridY = oldRoomY;

    setRoomBounds();

    roomRefreshPower();
}

int main(int argc, char *argv[]) {
    EDIT_MODE = argc > 1 && strcmp(argv[1], "edit") == 0;

    gbRendererInit("Test", 1, 1);
    gbInputInit();
    gbTextureInit();
    gbGfxInit();
    gbAnimationInit();
    dynamicEntityInit();
    collisionInit();
    guyInit();

    mainCabelsTexture = gbTextureLoadNamed(GB_TEXTURE_NAME_CABELS);

    if (EDIT_MODE)
        editorInit();
    else {
        moveRoomPanelInit();
        switchInit();
        gameInit();
    }

    if (!EDIT_MODE) {
        Position overlayPos = (Position){0, 0};
        Sprite overlay;
        spriteSet(&overlay,
                  gbTextureLoadNamed(GB_TEXTURE_NAME_OVERLAY),
                  0, 0, 1280, 720,
                  gbScreenWidth, gbScreenHeight,
                  SPRITE_LAYER_FOREGROUND,
                  1, 1, SDL_FLIP_NONE);

        spriteRegister(&overlay, &overlayPos);
    }

    uint8_t done = 0;
    uint32_t last_time = 0;
    uint32_t current_time = 0;
    double delta = 0;

    double secondCounter = 0;
    unsigned int fps = 0;
    double addAcc = 0;

    gbInputSetKey(GB_INPUT_MOVE_LEFT, SDLK_LEFT);
    gbInputSetKey(GB_INPUT_MOVE_RIGHT, SDLK_RIGHT);
    gbInputSetKey(GB_INPUT_JUMP, SDLK_SPACE);
    gbInputSetKey(GB_INPUT_INTERACT, SDLK_LSHIFT);
    gbInputSetKey(GB_INPUT_SELECT, SDLK_DOWN);
    gbInputSetKey(GB_INPUT_QUIT_GAME, SDLK_q);
    gbInputSetKey(GB_INPUT_MOUSE_SELECT, SDL_BUTTON_LEFT);
    gbInputSetKey(GB_INPUT_MOUSE_ALT, SDL_BUTTON_RIGHT);
    gbInputSetKey(GB_INPUT_TOGGLE_EDIT_MODE, SDLK_TAB);

    while (!done) {
        gbInputUpdate();

        done = gbInputCheckState(GB_INPUT_QUIT_GAME, GB_INPUT_RELEASED);

        if(gbInputCheckState(GB_INPUT_TOGGLE_EDIT_MODE, GB_INPUT_RELEASED))
            GB_GFX_DEBUG_FLAG = !GB_GFX_DEBUG_FLAG;

        if (EDIT_MODE) {
            editorUpdate();
            dynamicEntityAct(delta);
        } else {
            if (!handleRoomChange(delta)) {
                dynamicEntityAct(delta);
            }
        }

        gbGfxDraw();

        SDL_Delay(0);
        current_time = SDL_GetTicks();

        delta = (double)((current_time - last_time) * 0.001);
        last_time = current_time;
        secondCounter += delta;
        fps++;
        if (secondCounter > 1) {
            printf("FPS: %d\n", fps);
            fps = 0;
            secondCounter = 0;
        }

    }

//    file = SDL_RWFromFile("./player", "w");
//    if (file) {
//        SDL_RWwrite(file, guy, sizeof(uint8_t), sizeof(DynamicEntity));
//        SDL_RWclose(file);
//        file = 0;
//    }

    //editorTeardown();
    collisionTeardown();
    gbAnimationTeardown();
    gbGfxTeardown();
    gbTextureTeardown();
    gbInputTeardown();
    gbRendererTeardown();
    return 0;
}
