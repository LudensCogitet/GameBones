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
    uint8_t makeShips = 0;
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

        if (gb_input_check_state(GB_INPUT_THRUST, GB_INPUT_JUST_PRESSED) ||
            gb_input_check_state(GB_INPUT_THRUST, GB_INPUT_PRESSED | GB_INPUT_SHIFT)) {
            if (!makeShips) {
                if (shipsDead < 5) {
                    ship[shipsDead++]->dispose = 1;
                } else {
                    makeShips = 1;
                }
            }

            if (makeShips) {
                ship[--shipsDead] = gb_gfx_new_sprite(GFX_LAYER_MIDGROUND, GFX_TEXTURE_SHIP);

                ship[shipsDead]->src.w = 128;
                ship[shipsDead]->src.h = 128;

                ship[shipsDead]->dst.w = 64;
                ship[shipsDead]->dst.h = 64;

                ship[shipsDead]->dst.x = 64 * shipsDead;

                makeShips = shipsDead;
            }
        }

        gb_gfx_draw();
        SDL_Delay(0);
    }



    gb_gfx_teardown();
    gb_input_teardown();
    gb_destroy_main_renderer();
    return 0;
}
