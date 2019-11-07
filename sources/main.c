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

static float ship_direction[48][2] = {
{-0.000000, -1.000000},
{-0.130526, -0.991445},
{-0.258819, -0.965926},
{-0.382683, -0.923880},
{-0.500000, -0.866025},
{-0.608761, -0.793353},
{-0.707107, -0.707107},
{-0.793353, -0.608761},
{-0.866025, -0.500000},
{-0.923880, -0.382683},
{-0.965926, -0.258819},
{-0.991445, -0.130526},
{-1.000000, 0.000000},
{-0.991445, 0.130526},
{-0.965926, 0.258819},
{-0.923880, 0.382683},
{-0.866025, 0.500000},
{-0.793353, 0.608761},
{-0.707107, 0.707107},
{-0.608761, 0.793353},
{-0.500000, 0.866025},
{-0.382683, 0.923880},
{-0.258819, 0.965926},
{-0.130526, 0.991445},
{0.000000, 1.000000},
{0.130526, 0.991445},
{0.258819, 0.965926},
{0.382683, 0.923880},
{0.500000, 0.866025},
{0.608761, 0.793353},
{0.707107, 0.707107},
{0.793353, 0.608761},
{0.866025, 0.500000},
{0.923880, 0.382683},
{0.965926, 0.258819},
{0.991445, 0.130526},
{1.000000, 0.000000},
{0.991445, -0.130526},
{0.965926, -0.258819},
{0.923880, -0.382683},
{0.866025, -0.500000},
{0.793353, -0.608761},
{0.707107, -0.707107},
{0.608761, -0.793353},
{0.500000, -0.866025},
{0.382683, -0.923880},
{0.258819, -0.965926},
{0.130526, -0.991445}
};

int main(int argc, char *argv[]) {
    gb_init_main_renderer("GFX tests");
    gb_input_init();
    gb_gfx_init();

    gb_gfx_load_texture("./data/assets/ship.png", GFX_TEXTURE_SHIP);

    GbSprite *ship;
    GbSprite *fixedShip;
    GbAnimation *anim_rotate_ship;
    GbAnimation *anim_boost;
    GbAnimation *anim_thrust;

    fixedShip = gb_gfx_new_sprite(GFX_LAYER_MIDGROUND, GFX_TEXTURE_SHIP, 1);
    fixedShip->src.w = 128;
    fixedShip->src.h = 128;

    fixedShip->dst.w = 128;
    fixedShip->dst.h = 128;

    fixedShip->dst.x = 0;
    fixedShip->dst.y = 0;

    ship = gb_gfx_new_sprite(GFX_LAYER_MIDGROUND, GFX_TEXTURE_SHIP, 0);
    ship->src.w = 128;
    ship->src.h = 128;

    ship->dst.w = 32;
    ship->dst.h = 32;

    ship->dst.x = 300;
    ship->dst.y = 300;

    float * ship_dir = ship_direction[0];
    float dx = 0;
    float dy = 0;
    anim_rotate_ship = gb_anim_new_animation(0, 0, 128, 0, 21, 48, 1, ANIM_TYPE_LOOP);
    anim_boost = gb_anim_new_animation(0, 128, 0, 128, 50, 5, 1, ANIM_TYPE_LOOP);
    anim_thrust = gb_anim_new_animation(0, 128, 0, 128, 100, 3, -1, ANIM_TYPE_PINGPONG);

    gb_input_set_key(GB_INPUT_QUIT_GAME, SDLK_q);
    gb_input_set_key(GB_INPUT_ROTATE_LEFT, SDLK_a);
    gb_input_set_key(GB_INPUT_ROTATE_RIGHT, SDLK_d);
    gb_input_set_key(GB_INPUT_THRUST, SDLK_w);
    gb_input_set_key(GB_INPUT_BREAK, SDLK_SPACE);
    gb_input_set_key(GB_INPUT_SELECT, SDLK_RETURN);

    gb_input_set_key(GB_INPUT_PAN_CAMERA_UP, SDLK_UP);
    gb_input_set_key(GB_INPUT_PAN_CAMERA_DOWN, SDLK_DOWN);
    gb_input_set_key(GB_INPUT_PAN_CAMERA_LEFT, SDLK_LEFT);
    gb_input_set_key(GB_INPUT_PAN_CAMERA_RIGHT, SDLK_RIGHT);

    uint8_t done = 0;
    uint32_t last_time = 0;
    uint32_t current_time = 0;
    uint32_t delta = 0;
    double dDelta = 0;
    float acceleration = 300; // pixels per second
    float boostAcceleration = acceleration * 5;

    uint8_t boosting = 0;
    uint8_t thrusting = 0;

    double x = ship->dst.x;
    double y = ship->dst.y;

    int32_t c_dx = 0;
    int32_t c_dy = 0;

    while (!done) {
        gb_input_update();

        c_dx = 0;
        c_dy = 0;

        if (gb_input_check_state(GB_INPUT_PAN_CAMERA_LEFT, GB_INPUT_PRESSED)) {
            c_dx = -10;
        }
        if (gb_input_check_state(GB_INPUT_PAN_CAMERA_RIGHT, GB_INPUT_PRESSED)) {
            c_dx = 10;
        }
        if (gb_input_check_state(GB_INPUT_PAN_CAMERA_UP, GB_INPUT_PRESSED)) {
            c_dy = -10;
        }
        if (gb_input_check_state(GB_INPUT_PAN_CAMERA_DOWN, GB_INPUT_PRESSED)) {
            c_dy = 10;
        }

        if (gb_input_check_state(GB_INPUT_QUIT_GAME, GB_INPUT_JUST_PRESSED)) {
            done = 1;
        }

        if (gb_input_check_state(GB_INPUT_ROTATE_LEFT, GB_INPUT_PRESSED)) {
            anim_rotate_ship->direction = 1;
            gb_anim_apply(&ship->src, delta, anim_rotate_ship);
            ship_dir = ship_direction[anim_rotate_ship->current_frame];
        }

        if (gb_input_check_state(GB_INPUT_ROTATE_RIGHT, GB_INPUT_PRESSED)) {
            anim_rotate_ship->direction = -1;
            gb_anim_apply(&ship->src, delta, anim_rotate_ship);
            ship_dir = ship_direction[anim_rotate_ship->current_frame];
        }

        if (gb_input_check_state(GB_INPUT_THRUST, GB_INPUT_JUST_PRESSED)) {
            boosting = 1;
        }

        if (gb_input_check_state(GB_INPUT_THRUST, GB_INPUT_RELEASED)) {
            boosting = thrusting = anim_thrust->current_frame = anim_boost->current_frame = 0;
            anim_thrust->direction = -1;
            ship->src.y = 0;
        }

        if (gb_input_check_state(GB_INPUT_THRUST, GB_INPUT_PRESSED)) {
            dx += ship_dir[0] * (boosting ? boostAcceleration * dDelta : acceleration * dDelta);
            dy += ship_dir[1] * (boosting ? boostAcceleration * dDelta : acceleration * dDelta);
        }

        if (gb_input_check_state(GB_INPUT_BREAK, GB_INPUT_PRESSED)) {
            dx = dy = 0;
        }

        if (gb_input_check_state(GB_INPUT_SELECT, GB_INPUT_JUST_PRESSED)) {
            dx = 0;
            dy = 0;
            ship->dst.x = x = 300;
            ship->dst.y = y = 300;
        }

        if (boosting) {
            boosting = !gb_anim_apply(&ship->src, delta, anim_boost);
            if (!boosting) {
                thrusting = 1;
                anim_boost->current_frame = 0;
            }
        }

        if (thrusting) {
            gb_anim_apply(&ship->src, delta, anim_thrust);
        }

        x += dx * dDelta;
        y += dy * dDelta;

        printf("%f, %f\n", x, y);

        ship->dst.x = x;
        ship->dst.y = y;
        gb_gfx_camera_move(c_dx, c_dy);
        gb_gfx_draw();
        SDL_Delay(0);

        current_time = SDL_GetTicks();
        delta = current_time - last_time;
        dDelta = (double)(delta * 0.001);
        last_time = current_time;
    }

    free(anim_rotate_ship);
    free(anim_boost);
    free(anim_thrust);
    anim_rotate_ship = NULL;

    gb_gfx_teardown();
    gb_input_teardown();
    gb_destroy_main_renderer();
    return 0;
}
