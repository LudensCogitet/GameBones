#include <stdlib.h>
#include <stdio.h>

#include "./gbInput_type.h"
#include "./gbInput_sys.h"

static GbInputState *gbInputMap[GB_INPUT_NUM_INPUTS];

int gbInputInit() {
    for (uint8_t i = 0; i < GB_INPUT_NUM_INPUTS; i++) {
        gbInputMap[i] = (GbInputState *)malloc(sizeof(GbInputState));

        gbInputMap[i]->keycode = 0;
        gbInputMap[i]->state = 0;
        gbInputMap[i]->last_state = 0;
        gbInputMap[i]->timestamp = 0;
    }

    return 0;
}

void gbInputUpdate() {
    SDL_Event event;

    uint8_t numHandled = 0;
    uint8_t inputHandled[GB_INPUT_NUM_INPUTS] = { 0 };

    while (SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_QUIT:
                gbInputMap[GB_INPUT_QUIT_GAME]->state = GB_INPUT_JUST_PRESSED | GB_INPUT_SHIFT;
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

                    if (gbInputMap[i]->keycode == ((event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) ? event.key.keysym.sym : event.button.button)) {
                        gbInputMap[i]->last_state = gbInputMap[i]->state;
                        gbInputMap[i]->timestamp = SDL_GetTicks();

                        gbInputMap[i]->state = (event.type == SDL_KEYDOWN || event.type == SDL_MOUSEBUTTONDOWN) ? GB_INPUT_PRESSED | GB_INPUT_JUST_PRESSED : GB_INPUT_RELEASED;

                        if (gbInputMap[i]->state) {
                            SDL_Keymod mod = SDL_GetModState();

                            if (mod & KMOD_ALT) gbInputMap[i]->state |= GB_INPUT_ALT;
                            if (mod & KMOD_CTRL) gbInputMap[i]->state |= GB_INPUT_CTRL;
                            if (mod & KMOD_SHIFT) gbInputMap[i]->state |= GB_INPUT_SHIFT;
                        }

                        inputHandled[i] = 1;
                        numHandled++;
                    }
                }
        }
    }

    for (uint8_t i = 0; i < GB_INPUT_NUM_INPUTS; i++) {
        if (inputHandled[i]) continue;

        gbInputMap[i]->state &= ~(GB_INPUT_JUST_PRESSED | GB_INPUT_RELEASED);
    }
}

void gbInputTeardown() {
    for (uint8_t i = 0; i < GB_INPUT_NUM_INPUTS; i++) {
        free(gbInputMap[i]);
        gbInputMap[i] = 0;
    }
}

void gbInputSetKey(GB_INPUT input, uint32_t key) {
    gbInputMap[input]->keycode = key;
}

GbInputState gbInputGetState(GB_INPUT input) {
    return *gbInputMap[input];
}

uint8_t gbInputCheckState(GB_INPUT input, uint8_t mask) {
    return (gbInputMap[input]->state & mask) == mask;
}

void gbInputClearState(GB_INPUT input) {
    gbInputMap[input]->last_state = gbInputMap[input]->state;
    gbInputMap[input]->timestamp = SDL_GetTicks();
    gbInputMap[input]->state = 0;
}

//void gb_input_get_mouse_pos_screen(int *x, int *y) {
//    SDL_GetMouseState(x, y);
//    gb_gfx_screen_to_world_coords(x, y);
//}
//
//void gb_input_get_mouse_pos_world(int *x, int *y) {
//    int cx, cy;
//
//    gb_gfx_camera_get_pos(&cx, &cy);
//    SDL_GetMouseState(x, y);
//
//    *x += cx;
//    *y += cy;
//
//    gb_gfx_screen_to_world_coords(x, y);
//}
