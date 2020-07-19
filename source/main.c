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

int main(int argc, char *argv[]) {
    gbRendererInit("Test", 1, 1);
    gbInputInit();
    gbTextureInit();
    gbGfxInit();
    gbAnimationInit();
    dynamicEntityInit();
    collisionInit();
    editorInit();

    dynamicEntityInits[DYNAMIC_ENTITY_TYPE_GUY]();

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

        editorUpdate();
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

    editorTeardown();
    collisionTeardown();
    gbAnimationTeardown();
    gbGfxTeardown();
    gbTextureTeardown();
    gbInputTeardown();
    gbRendererTeardown();
    return 0;
}
