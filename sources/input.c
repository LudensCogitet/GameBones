#include <stdlib.h>
#include <stdio.h>
#include "../headers/input.h"

static GbInputState *gb_input_map[GB_INPUT_NUM_INPUTS];

int gb_init_input() {
    for (uint8_t i = 0; i < GB_INPUT_NUM_INPUTS; i++) {
        gb_input_map[i] = (GbInputState *)malloc(sizeof(GbInputState));

        gb_input_map[i]->keycode = 0;
        gb_input_map[i]->state = 0;
        gb_input_map[i]->last_state = 0;
        gb_input_map[i]->timestamp = 0;
    }

    gb_input_map[GB_INPUT_THRUST]->keycode = SDLK_w;
    gb_input_map[GB_INPUT_ROTATE_LEFT]->keycode = SDLK_a;
    gb_input_map[GB_INPUT_ROTATE_RIGHT]->keycode = SDLK_d;
    gb_input_map[GB_INPUT_BREAK]->keycode = SDLK_s;

    gb_input_map[GB_INPUT_SELECT]->keycode = SDL_BUTTON_LEFT;
    gb_input_map[GB_INPUT_BE_AWESOME]-> keycode = SDLK_b;

    gb_input_map[GB_INPUT_QUIT_GAME]->keycode = SDLK_q;

    return 0;
}

void gb_update_input() {
    SDL_Event event;

    uint8_t numHandled = 0;
    uint8_t inputHandled[GB_INPUT_NUM_INPUTS] = { 0 };

    while (SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_QUIT:
                gb_input_map[GB_INPUT_QUIT_GAME]->state = GB_INPUT_JUST_PRESSED | GB_INPUT_SHIFT;
                inputHandled[GB_INPUT_QUIT_GAME] = 1;
                break;
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                if (event.key.repeat) continue;
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
                if (numHandled == GB_INPUT_NUM_INPUTS) continue;

                for (uint8_t i = 0; i < GB_INPUT_NUM_INPUTS; i++) {
                    if (inputHandled[i]) continue;

                    if (gb_input_map[i]->keycode == ((event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) ? event.key.keysym.sym : event.button.button)) {
                        gb_input_map[i]->last_state = gb_input_map[i]->state;
                        gb_input_map[i]->timestamp = SDL_GetTicks();

                        gb_input_map[i]->state = (event.type == SDL_KEYDOWN || event.type == SDL_MOUSEBUTTONDOWN) ? GB_INPUT_PRESSED | GB_INPUT_JUST_PRESSED : 0;

                        if (gb_input_map[i]->state) {
                            SDL_Keymod mod = SDL_GetModState();

                            if (mod & KMOD_ALT) gb_input_map[i]->state |= GB_INPUT_ALT;
                            if (mod & KMOD_CTRL) gb_input_map[i]->state |= GB_INPUT_CTRL;
                            if (mod & KMOD_SHIFT) gb_input_map[i]->state |= GB_INPUT_SHIFT;
                        }

                        inputHandled[i] = 1;
                        numHandled++;
                    }
                }
        }
    }

    for (uint8_t i = 0; i < GB_INPUT_NUM_INPUTS; i++) {
        if (inputHandled[i]) continue;

        gb_input_map[i]->state &= (~GB_INPUT_JUST_PRESSED);
    }
}

void gb_teardown_input() {
    for (uint8_t i = 0; i < GB_INPUT_NUM_INPUTS; i++) {
        free(gb_input_map[i]);
        gb_input_map[i] = NULL;
    }
}

GbInputState gb_get_input_state(GB_INPUT input) {
    return *gb_input_map[input];
}

uint8_t gb_check_input_state(GB_INPUT input, uint8_t mask) {
    return (gb_input_map[input]->state & mask) == mask;
}
