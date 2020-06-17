#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include <SDL2/SDL.h>

#include "./gbRenderer/gbRenderer_sys.h"
#include "./gbInput/gbInput_sys.h"
#include "./gbGfx/gbGfx_sys.h"
#include "./gbAnimation/gbAnimation_sys.h"
#include "./gbCollision/gbCollision_sys.h"

#include "./gbTexture/gbTexture_sys.h"
#include "./gbSerializer/gbSerializer_sys.h"

#include "./gbInput/gbInput_type.h"
#include "./gbGfx/gbSprite_type.h"
#include "./gbAnimation/gbAnimation_type.h"
#include "./gbAnimation/gbAnimType_type.h"
#include "./gbEntity/gbEntity_sys.h"
#include "./editor/editor.h";

#include "./gbSerializer/gbFile_type.h"

#include "./entities/Guy/entityGuy.h"


int main(int argc, char *argv[]) {
    gbRendererInit("Test", 1, 1);
    gbInputInit();
    gbTextureInit();
    gbGfxInit();
    gbAnimationInit();
    gbCollisionInit();

    initGuy();

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

    Guy *guy;

    gbFile *file = gbSerializerOpenFileRead("./gameSave.sav");

    if (file) {
        gbSerializerReadChunk(file, GB_FILE_CHUNK_SIZE_16);
        guy = (Guy *)gbEntityDeserialize[GB_ENTITY_TYPE_GUY](file);
        gbSerializerCloseFile(file);
    } else {
        SDL_ClearError();
        guy = guyNew(GB_GFX_GRID_OFFSET_X + (GB_GFX_GRID_SIZE * 3), GB_GFX_GRID_OFFSET_Y + (GB_GFX_GRID_SIZE * 3), SDL_FLIP_NONE)->entity;
    }

    int x1, y1, x2, y2;
    gbGfxGridSquareToWorldCoords(0, 8, &x1, &y1);
    gbGfxGridSquareToWorldCoords(10, 9, &x2, &y2);

    gbCollisionStaticColliderNew(x1, y1, x2, y2);

    gbGfxGridSquareToWorldCoords(16, 8, &x1, &y1);
    gbGfxGridSquareToWorldCoords(20, 9, &x2, &y2);
    gbCollisionStaticColliderNew(x1, y1, x2, y2);

    gbGfxGridSquareToWorldCoords(0, 0, &x1, &y1);
    gbGfxGridSquareToWorldCoords(1, 9, &x2, &y2);
    gbCollisionStaticColliderNew(x1, y1, x2, y2);

    gbGfxGridSquareToWorldCoords(19, 0, &x1, &y1);
    gbGfxGridSquareToWorldCoords(20, 9, &x2, &y2);
    gbCollisionStaticColliderNew(x1, y1, x2, y2);

    gbGfxGridSquareToWorldCoords(0, 11, &x1, &y1);
    gbGfxGridSquareToWorldCoords(20, 12, &x2, &y2);
    gbCollisionStaticColliderNew(x1, y1, x2, y2);

    gbGfxGridSquareToWorldCoords(13, 9, &x1, &y1);
    gbGfxGridSquareToWorldCoords(14, 12, &x2, &y2);
    gbCollisionStaticColliderNew(x1, y1, x2, y2);

    while (!done) {
        gbInputUpdate();

        done = gbInputCheckState(GB_INPUT_QUIT_GAME, GB_INPUT_RELEASED);

        if(gbInputCheckState(GB_INPUT_MOUSE_SELECT, GB_INPUT_JUST_PRESSED)) {
            int x, y;
            SDL_GetMouseState(&x, &y);
            gbGfxScreenCoordsToGridSquare(x, y, &x, &y);
            if (x > -1 && y > -1) {
                gbGfxGridSquareToWorldCoords(x, y, &x, &y);
                guy->pos.x = x;
                guy->pos.y = y;
            }
        }

        if(gbInputCheckState(GB_INPUT_TOGGLE_EDIT_MODE, GB_INPUT_RELEASED))
            editorEditModeOn = !editorEditModeOn;

        editorUpdate();
        if (!editorEditModeOn) gbEntityAct(delta);
        gbGfxDraw();

        SDL_Delay(0);
        current_time = SDL_GetTicks();

        delta = (double)((current_time - last_time) * 0.001);
        last_time = current_time;
        secondCounter += delta;
        fps++;
        if (secondCounter > 1) {
            //printf("FPS: %d\n", fps);
            fps = 0;
            secondCounter = 0;
        }

    }

    file = gbSerializerOpenFileWrite("./gameSave.sav");
    gbEntitySerialize[GB_ENTITY_TYPE_GUY](guy, file);
    gbSerializerCloseFile(file);

    gbCollisionTeardown();
    gbAnimationTeardown();
    gbGfxTeardown();
    gbTextureTeardown();
    gbInputTeardown();
    gbRendererTeardown();
    return 0;
}
