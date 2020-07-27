#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
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

static int activeRoomX = 1;
static int activeRoomY = 0;
static Room *rooms[2][2];

DynamicEntity *mainPlayer = 0;
void setPlayerPosition(double x, double y) {
    if (!mainPlayer) {
        mainPlayer = guyNew(x, y);
        dynamicEntityRegister(mainPlayer);
    } else {
        mainPlayer->pos.x = x;
        mainPlayer->pos.y = y;
    }
}

void gameInit() {
    for (unsigned int x = 0; x < 2; x++) {
        for (unsigned int y = 0; y < 2; y++) {
            rooms[x][y] = roomNew();
        }
    }
    roomDeserialize(rooms[1][0], "./room1.rm");
    roomDeserialize(rooms[1][1], "./room2.rm");
    roomDeserialize(rooms[0][1], "./room3.rm");
    roomDeserialize(rooms[0][0], "./room4.rm");

    roomActivate(rooms[activeRoomX][activeRoomY]);
}

void checkRoomChange() {
    double pLeftExtremity = (mainPlayer->pos.x);
    double pRightExtremity = mainPlayer->pos.x + mainPlayer->sprite.width;

    double pTopExtremity = mainPlayer->pos.y;
    double pBottomExtremity = mainPlayer->pos.y + mainPlayer->sprite.height;

    int oldRoomX = -1;
    int oldRoomY = -1;

    // Player has exited grid to the left
    if (pRightExtremity < GB_GFX_GRID_OFFSET_X) {
        oldRoomX = activeRoomX;
        oldRoomY = activeRoomY;
        if (activeRoomX == 0)
            activeRoomX = 1;
        else
            activeRoomX = 0;

        mainPlayer->pos.x += GB_GFX_GRID_SIZE * GB_GFX_GRID_WIDTH;
    }

    // Player has exited grid to the right
    if (pLeftExtremity > (GB_GFX_GRID_OFFSET_X + (GB_GFX_GRID_SIZE * GB_GFX_GRID_WIDTH))) {
        oldRoomX = activeRoomX;
        oldRoomY = activeRoomY;
        if (activeRoomX == 1)
            activeRoomX = 0;
        else
            activeRoomX = 1;

        mainPlayer->pos.x = GB_GFX_GRID_OFFSET_X;
    }

    // Player has exited grid by the top
    if (pBottomExtremity < GB_GFX_GRID_OFFSET_Y) {
        oldRoomX = activeRoomX;
        oldRoomY = activeRoomY;
        if (activeRoomY == 0)
            activeRoomY = 1;
        else
            activeRoomY = 0;

        mainPlayer->pos.y += GB_GFX_GRID_SIZE * GB_GFX_GRID_HEIGHT;
    }

    // Player has exited grid through the bottom
    if (pTopExtremity > (GB_GFX_GRID_OFFSET_Y + (GB_GFX_GRID_SIZE * GB_GFX_GRID_HEIGHT))) {
        oldRoomX = activeRoomX;
        oldRoomY = activeRoomY;
        if (activeRoomY == 1)
            activeRoomY = 0;
        else
            activeRoomY = 1;

        mainPlayer->pos.y = GB_GFX_GRID_OFFSET_Y;
    }

    if (oldRoomX > -1 && oldRoomY > -1) {
        roomDeactivate(rooms[oldRoomX][oldRoomY]);
        roomActivate(rooms[activeRoomX][activeRoomY]);
    }
}

int main(int argc, char *argv[]) {
    gbRendererInit("Test", 1, 1);
    gbInputInit();
    gbTextureInit();
    gbGfxInit();
    gbAnimationInit();
    dynamicEntityInit();
    collisionInit();
    guyInit();

    gameInit();//editorInit();

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
    gbInputSetKey(GB_INPUT_QUIT_GAME, SDLK_q);
    gbInputSetKey(GB_INPUT_MOUSE_SELECT, SDL_BUTTON_LEFT);
    gbInputSetKey(GB_INPUT_MOUSE_ALT, SDL_BUTTON_RIGHT);
    gbInputSetKey(GB_INPUT_TOGGLE_EDIT_MODE, SDLK_TAB);

    while (!done) {
        gbInputUpdate();

        done = gbInputCheckState(GB_INPUT_QUIT_GAME, GB_INPUT_RELEASED);

        if(gbInputCheckState(GB_INPUT_TOGGLE_EDIT_MODE, GB_INPUT_RELEASED))
            GB_GFX_DEBUG_FLAG = !GB_GFX_DEBUG_FLAG;

        checkRoomChange();//editorUpdate();
        dynamicEntityAct(delta);
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
