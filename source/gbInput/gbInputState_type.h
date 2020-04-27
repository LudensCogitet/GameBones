#ifndef GB_INPUT_STATE_TYPE_H_INCLUDED
#define GB_INPUT_STATE_TYPE_H_INCLUDED

#include "SDL2/SDL.h"

typedef struct {
    SDL_Keycode keycode;

    uint8_t state;
    uint8_t last_state;

    uint32_t timestamp;
} GbInputState;

#endif // GB_INPUT_STATE_TYPE_H_INCLUDED
