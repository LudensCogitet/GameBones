#ifndef INPUT_H_INCLUDED
#define INPUT_H_INCLUDED

#include <stdint.h>
#include <SDL2/SDL.h>
#include "../headers/enums.h"

#define GB_INPUT_PRESSED          1
#define GB_INPUT_JUST_PRESSED     2
#define GB_INPUT_RELEASED         4
#define GB_INPUT_SHIFT            8
#define GB_INPUT_ALT             16
#define GB_INPUT_CTRL            32
#define GB_INPUT_DOUBLE_PRESSED  64

typedef struct GbInputState {
    SDL_Keycode keycode;

    uint8_t state;
    uint8_t last_state;

    uint32_t timestamp;
} GbInputState;

int gb_input_init();
void gb_input_update();
void gb_input_teardown();

GbInputState gb_input_get_state(GB_INPUT input);
uint8_t gb_input_check_state(GB_INPUT input, uint8_t mask);
void gb_input_set_key(GB_INPUT input, uint32_t key);

#endif
