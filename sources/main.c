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
#include "../headers/entity.h"
#include "../headers/entities/player_ship.h"
#include "../headers/entities/asteroid.h"
#include "../headers/physics.h"

int main(int argc, char *argv[]) {
    gb_init_main_renderer("Logic Tests");
    gb_input_init();
    gb_gfx_init();
    gb_physics_init();
    gb_entity_init();

    gb_gfx_texture_load("./data/assets/ship.png", GFX_TEXTURE_SHIP);
    gb_gfx_texture_load("./data/assets/asteroid.png", GFX_TEXTURE_ASTEROID);

    PlayerShip * player = player_ship_new(LOGICAL_SCREEN_WIDTH / 2, LOGICAL_SCREEN_HEIGHT / 2, 0, 300, 1000);
    asteroid_new(1000, 1000, 4);
    asteroid_new(234, 165, 7);
    asteroid_new(600, 500, 2);
    Asteroid * asteroid = asteroid_new(100, 100, 20);
    uint8_t toggleFollow = 0;
    gb_gfx_camera_follow(&asteroid->body->x, &asteroid->body->y, LOGICAL_SCREEN_WIDTH * 0.4, LOGICAL_SCREEN_HEIGHT * 0.4);

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

        if (gb_input_check_state(GB_INPUT_BREAK, GB_INPUT_JUST_PRESSED)) {
            if (toggleFollow)
                gb_gfx_camera_follow(&asteroid->body->x, &asteroid->body->y, LOGICAL_SCREEN_WIDTH * 0.4, LOGICAL_SCREEN_HEIGHT * 0.4);
            else
                gb_gfx_camera_unfollow();

            toggleFollow = !toggleFollow;
        }

        gb_entity_act(delta);
        gb_gfx_camera_move(c_dx, c_dy);
        gb_gfx_draw();
        SDL_Delay(0);

        current_time = SDL_GetTicks();
        delta = current_time - last_time;
        last_time = current_time;
    }
    gb_gfx_camera_unfollow();
    gb_entity_teardown();
    gb_physics_teardown();
    gb_gfx_teardown();
    gb_input_teardown();
    gb_destroy_main_renderer();
    return 0;
}
