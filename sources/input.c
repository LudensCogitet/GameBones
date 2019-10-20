#include <stdlib.h>
#include <stdio.h>
#include "../headers/input.h"

static GbInputState *gb_input_map[GB_NUM_INPUTS];

int gb_init_input() {
    for (uint8_t i = 0; i < GB_NUM_INPUTS; i++) {
        gb_input_map[i] = (GbInputState *)malloc(sizeof(GbInputState));

        gb_input_map[i]->keycode = 0;
        gb_input_map[i]->state = 0;
        gb_input_map[i]->last_state = 0;
        gb_input_map[i]->timestamp = 0;
    }

    gb_input_map[GB_THRUST]->keycode = SDL_BUTTON_LEFT;
    gb_input_map[GB_QUIT_GAME]->keycode = SDLK_q;

    return 0;
}

void gb_update_input() {
    SDL_Event event;
    static uint32_t codes[10];
    static uint8_t states[10];
    static uint32_t timestamps[10];

    uint8_t index = 0;

    while (SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_KEYDOWN:
            case SDL_KEYUP:
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
                if (index >= 10) continue;

                states[index] = 0;
                if ((event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) && !event.key.repeat) {
                    codes[index] = event.key.keysym.sym;
                    if (event.type == SDL_KEYDOWN) {
                        states[index] = GB_INPUT_PRESSED | GB_INPUT_JUST_PRESSED;
                    }
                } else {
                    codes[index] = event.button.button;
                    if (event.type == SDL_MOUSEBUTTONDOWN) {
                        states[index] = GB_INPUT_PRESSED | GB_INPUT_JUST_PRESSED;
                    }
                }

                SDL_Keymod mod = SDL_GetModState();

                if (mod & KMOD_ALT) states[index] |= GB_INPUT_ALT;
                if (mod & KMOD_CTRL) states[index] |= GB_INPUT_CTRL;
                if (mod & KMOD_SHIFT) states[index] |= GB_INPUT_SHIFT;
                printf("%d\n", states[index]);
                index++;
        }
    }

    for (uint8_t i = 0; i < GB_NUM_INPUTS; i++) {
        for (uint8_t j = 0; j < index; j++) {

            if (gb_input_map[i]->keycode == codes[j]) {
                gb_input_map[i]->last_state = gb_input_map[i]->state;
                gb_input_map[i]->state = states[j];
                gb_input_map[i]->timestamp = timestamps[j];
            } else {
                gb_input_map[i]->state &= ~GB_INPUT_JUST_PRESSED;
            }
        }
    }
}

void gb_teardown_input() {
    for (uint8_t i = 0; i < GB_NUM_INPUTS; i++) {
        free(gb_input_map[i]);
        gb_input_map[i] = NULL;
    }
}

GbInputState gb_get_input_state(GB_INPUT input) {
    return *gb_input_map[input];
}
