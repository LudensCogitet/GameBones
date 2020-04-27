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
    double scaleAcc = 0;

    gbInputSetKey(GB_INPUT_MOVE_LEFT, SDLK_LEFT);
    gbInputSetKey(GB_INPUT_MOVE_RIGHT, SDLK_RIGHT);
    gbInputSetKey(GB_INPUT_SCALE_UP, SDLK_EQUALS);
    gbInputSetKey(GB_INPUT_SCALE_DOWN, SDLK_MINUS);
    gbInputSetKey(GB_INPUT_QUIT_GAME, SDLK_q);
    gbInputSetKey(GB_INPUT_DESTROY_DINOS, SDL_BUTTON_LEFT);

    for (unsigned int y = 0; y < 14; y++) {
        for (unsigned int x = 0; x < 25; x++) {
            dinoNew(x * 24, y * 24);
        }
    }


//    unsigned int animation = gbAnimationNew(
//                                         0, 0,
//                                         24, 0,
//                                         4,
//                                         3,
//                                         1,
//                                         GB_ANIM_TYPE_LOOP);
//
//    gbAnimationState animState = gbAnimationStateNew(animation);

    while (!done) {
        gbInputUpdate();

        done = gbInputCheckState(GB_INPUT_QUIT_GAME, GB_INPUT_RELEASED);
//
//        if (gbInputCheckState(GB_INPUT_SCALE_UP, GB_INPUT_PRESSED)) {
//            sprite->width += 1;
//            sprite->height += 1;
//            printf("w: %d, w: %d\n", sprite->width, sprite->height);
//        }
//
//        if (gbInputCheckState(GB_INPUT_SCALE_DOWN, GB_INPUT_PRESSED)) {
//            sprite->width -= 1;
//            sprite->height -= 1;
//            printf("w: %d, w: %d\n", sprite->width, sprite->height);
//        }
//
//
//        if (gbInputCheckState(GB_INPUT_SCALE_UP, GB_INPUT_RELEASED) || gbInputCheckState(GB_INPUT_SCALE_DOWN, GB_INPUT_RELEASED)) {
//            scaleAcc = 0;
//        }
//
//        gbAnimationApply(&sprite->src, delta, &animState, animation);
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
