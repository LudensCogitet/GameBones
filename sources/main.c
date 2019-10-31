#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include <SDL2/SDL.h>

#include "../headers/renderer.h"
#include "../headers/input.h"
#include "../headers/gfx.h"
#include "../headers/textures.h"
#include "../headers/animation.h"

int main(int argc, char *argv[]) {
    gb_init_main_renderer("GFX tests");
    gb_input_init();
    gb_gfx_init();

    gb_gfx_load_texture("./data/assets/ship_frames.png", GFX_TEXTURE_SHIP);

    GbSprite *ship;
    GbAnimation *anim;

    ship = gb_gfx_new_sprite(GFX_LAYER_MIDGROUND, GFX_TEXTURE_SHIP);
    ship->src.w = 128;
    ship->src.h = 128;

    ship->dst.w = 128;
    ship->dst.h = 128;

    ship->dst.x = 300;
    ship->dst.y = 300;

    anim = gb_anim_new_animation(0, 0, 128, 0, 50, 48, 1, ANIM_TYPE_LOOP);

    uint16_t dr = 50;

    gb_input_set_key(GB_INPUT_QUIT_GAME, SDLK_q);
    gb_input_set_key(GB_INPUT_ROTATE_LEFT, SDLK_a);
    gb_input_set_key(GB_INPUT_ROTATE_RIGHT, SDLK_d);

    uint8_t done = 0;
    uint32_t last_time = 0;
    uint32_t current_time = 0;
    uint32_t delta = 0;
    while (!done) {
        gb_input_update();

        if (gb_input_check_state(GB_INPUT_QUIT_GAME, GB_INPUT_JUST_PRESSED)) {
            done = 1;
        }

        if (gb_input_check_state(GB_INPUT_ROTATE_LEFT, GB_INPUT_PRESSED)) {
            anim->direction = 1;
            gb_anim_apply(&ship->src, delta, anim);
        }

        if (gb_input_check_state(GB_INPUT_ROTATE_RIGHT, GB_INPUT_PRESSED)) {
            anim->direction = -1;
            gb_anim_apply(&ship->src, delta, anim);
        }

//        if (gb_anim_apply(&ship->src, delta, anim)) {
//          printf("animation complete\n");
//        }


        gb_gfx_draw();
        SDL_Delay(0);

        current_time = SDL_GetTicks();
        delta = current_time - last_time;
        last_time = current_time;
    }

    free(anim);
    anim = NULL;

    gb_gfx_teardown();
    gb_input_teardown();
    gb_destroy_main_renderer();
    return 0;
}
