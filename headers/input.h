#ifndef INPUT_H_INCLUDED
#define INPUT_H_INCLUDED

#include <stdint.h>
#include <SDL2/SDL.h>

#define GB_INPUT_PRESSED          1
#define GB_INPUT_JUST_PRESSED     2
#define GB_INPUT_SHIFT            4
#define GB_INPUT_ALT              8
#define GB_INPUT_CTRL            16
#define GB_INPUT_DOUBLE_PRESSED  32

typedef enum GB_INPUT {
    GB_THRUST,
    GB_BREAK,
    GB_ROTATE_LEFT,
    GB_ROTATE_RIGHT,
    GB_QUIT_GAME,
    GB_NUM_INPUTS
} GB_INPUT;

typedef struct GbInputState {
    SDL_Keycode keycode;

    uint8_t state;
    uint8_t last_state;

    uint32_t timestamp;
} GbInputState;

int gb_init_input();
void gb_update_input();
void gb_teardown_input();

GbInputState gb_get_input_state(GB_INPUT input);

#endif
