#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include "../headers/renderer.h"
#include "../headers/input.h"
#include "../headers/gfx.h"
#include "../headers/textures.h"
#include "../headers/animation.h"
#include "../headers/entity.h"
#include "../headers/entities/player_ship.h"
#include "../headers/entities/asteroid.h"
#include "../headers/physics.h"
#include "../headers/sfx.h"

int main(int argc, char *argv[]) {
    gb_init_main_renderer("Logic Tests");
    gb_input_init();
    gb_gfx_init();
    gb_physics_init();
    gb_entity_init();
    gb_sfx_init();

    gb_gfx_texture_load("./data/assets/ship.png", GFX_TEXTURE_SHIP);
    gb_gfx_texture_load("./data/assets/asteroid.png", GFX_TEXTURE_ASTEROID);
    gb_gfx_texture_load("./data/assets/space_background.png", GFX_TEXTURE_BACKGROUND);
    gb_gfx_texture_load("./data/assets/temp_bullet.png", GFX_TEXTURE_BULLET);

    gb_sfx_sound_load("./data/assets/player_ship_boost.wav", SFX_SOUND_SHIP_BOOST);
    gb_sfx_sound_load("./data/assets/player_ship_cruise.wav", SFX_SOUND_SHIP_CRUISE);
    gb_sfx_sound_load("./data/assets/crash.wav", SFX_SOUND_SHIP_CRASH);
    gb_sfx_sound_load("./data/assets/asteroid_hit.wav", SFX_SOUND_ASTEROID_HIT);

    PlayerShip * player = player_ship_new(LOGICAL_SCREEN_WIDTH / 2, LOGICAL_SCREEN_HEIGHT / 2, 0, 300, 1000);
    asteroid_new(1000, 1000, 4, 0);
    asteroid_new(234, 165, 7, 0);
    asteroid_new(600, 500, 2, 0);

    gb_gfx_font_load("./data/assets/FreeMono.ttf", GFX_FONT_LARGE_FREE_MONO, 50);
    gb_gfx_font_set(GFX_FONT_LARGE_FREE_MONO);
    gb_gfx_font_color_set(GFX_COLOR_WHITE);
    gb_gfx_font_layer_set(GFX_LAYER_MIDGROUND);
    gb_gfx_new_text("Hello. This is an example of my text generation code.", 500, 0, 0, GFX_ANCHOR_TOP | GFX_ANCHOR_LEFT , 1);

    Asteroid * asteroid = asteroid_new(100, 100, 20, 0);
    uint8_t toggleFollow = 0;
    gb_gfx_camera_follow(&player->body->x, &player->body->y, LOGICAL_SCREEN_WIDTH * 0.4, LOGICAL_SCREEN_HEIGHT * 0.4);

    gb_gfx_new_sprite(GFX_LAYER_BACKGROUND, GFX_TEXTURE_BACKGROUND, 0, 0, 1920, 1080, LOGICAL_SCREEN_WIDTH / 2, LOGICAL_SCREEN_HEIGHT / 2, GFX_ANCHOR_DEFAULT, LOGICAL_SCREEN_WIDTH, LOGICAL_SCREEN_HEIGHT, 1);

    gb_input_set_key(GB_INPUT_QUIT_GAME, SDLK_q);
    gb_input_set_key(GB_INPUT_ROTATE_LEFT, SDLK_a);
    gb_input_set_key(GB_INPUT_ROTATE_RIGHT, SDLK_d);
    gb_input_set_key(GB_INPUT_THRUST, SDLK_w);
    gb_input_set_key(GB_INPUT_FIRE, SDLK_SPACE);
    gb_input_set_key(GB_INPUT_SELECT, SDL_BUTTON_LEFT);

    gb_input_set_key(GB_INPUT_PAN_CAMERA_UP, SDLK_UP);
    gb_input_set_key(GB_INPUT_PAN_CAMERA_DOWN, SDLK_DOWN);
    gb_input_set_key(GB_INPUT_PAN_CAMERA_LEFT, SDLK_LEFT);
    gb_input_set_key(GB_INPUT_PAN_CAMERA_RIGHT, SDLK_RIGHT);

    uint8_t done = 0;
    uint32_t last_time = 0;
    uint32_t current_time = 0;
    double delta = 0;

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
                gb_gfx_camera_follow(&asteroid->body->x, &asteroid->body->y, LOGICAL_SCREEN_WIDTH * 0.5, LOGICAL_SCREEN_HEIGHT * 0.5);
            else
                gb_gfx_camera_follow(&player->body->x, &player->body->y, LOGICAL_SCREEN_WIDTH * 0.4, LOGICAL_SCREEN_HEIGHT * 0.4);

            toggleFollow = !toggleFollow;
        }

        gb_entity_act(delta);
        gb_physics_detect_collisions(delta);
        gb_entity_message_handle();
        gb_gfx_camera_move(c_dx, c_dy);
        gb_gfx_draw();
        SDL_Delay(0);

        current_time = SDL_GetTicks();
        delta = (double)((current_time - last_time) * 0.001);
        last_time = current_time;
    }
    gb_gfx_camera_unfollow();
    gb_gfx_font_unload(GFX_FONT_LARGE_FREE_MONO);
    gb_sfx_teardown();
    gb_entity_teardown();
    gb_physics_teardown();
    gb_gfx_teardown();
    gb_input_teardown();
    gb_destroy_main_renderer();
    return 0;
}
