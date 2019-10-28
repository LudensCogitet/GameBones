#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include <SDL2/SDL.h>

#include "../headers/renderer.h"
#include "../headers/input.h"
#include "../headers/gfx.h"
#include "../headers/textures.h"

int main(int argc, char *argv[]) {
    gb_init_main_renderer("GFX tests");
    gb_input_init();
    gb_gfx_init();

    gb_gfx_load_texture("./data/assets/ship_frames.png", GFX_TEXTURE_SHIP);

    GbSprite *ship[5];
    int shipsDead = 0;
    for (int i = 0; i < 5; i++) {
        ship[i] = gb_gfx_new_sprite(GFX_LAYER_MIDGROUND, GFX_TEXTURE_SHIP);
        ship[i]->src.w = 128;
        ship[i]->src.h = 128;

        ship[i]->dst.w = 64;
        ship[i]->dst.h = 64;

        ship[i]->dst.x = 64 * i;
    }

    gb_input_set_key(GB_INPUT_QUIT_GAME, SDLK_q);
    gb_input_set_key(GB_INPUT_THRUST, SDLK_RETURN);

    uint8_t done = 0;
    while (!done) {
        gb_input_update();

        if (gb_input_check_state(GB_INPUT_QUIT_GAME, GB_INPUT_JUST_PRESSED)) {
            done = 1;
        }

        if (shipsDead < 5 && gb_input_check_state(GB_INPUT_THRUST, GB_INPUT_JUST_PRESSED)) {
            ship[shipsDead++]->dispose = 1;
        }

        gb_gfx_draw();
        SDL_Delay(0);
    }



    gb_gfx_teardown();
    gb_input_teardown();
    gb_destroy_main_renderer();
    return 0;
}
