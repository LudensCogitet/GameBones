#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include <SDL2/SDL.h>

#include "./gbRenderer/gbRenderer_sys.h"
#include "./gbInput/gbInput_sys.h"
#include "./gbGfx/gbGfx_sys.h"
#include "./gbAnimation/gbAnimation_sys.h"

#include "./gbTexture/gbTexture_sys.h"

#include "./gbInput/gbInput_type.h"
#include "./gbGfx/gbSprite_type.h"
#include "./gbAnimation/gbAnimation_type.h"
#include "./gbAnimation/gbAnimType_type.h"
#include "./gbEntity/gbEntity_sys.h"

#include "./entities/Dino/entityDino.h"


int main(int argc, char *argv[]) {
    gbRendererInit("Test", 0, 1);
    gbInputInit();
    gbTextureInit();
    gbGfxInit();
    gbAnimationInit();

    uint8_t done = 0;
    uint32_t last_time = 0;
    uint32_t current_time = 0;
    double delta = 0;

    double secondCounter = 0;
    unsigned int fps = 0;
    double addAcc = 0;

    gbInputSetKey(GB_INPUT_MOVE_LEFT, SDLK_LEFT);
    gbInputSetKey(GB_INPUT_MOVE_RIGHT, SDLK_RIGHT);
    gbInputSetKey(GB_INPUT_QUIT_GAME, SDLK_q);

    dinoNew(0, 300, SDL_FLIP_NONE);

    while (!done) {
        gbInputUpdate();

        done = gbInputCheckState(GB_INPUT_QUIT_GAME, GB_INPUT_RELEASED);

        gbEntityAct(delta);
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

    gbAnimationTeardown();
    gbGfxTeardown();
    gbTextureTeardown();
    gbInputTeardown();
    gbRendererTeardown();
    return 0;
}
