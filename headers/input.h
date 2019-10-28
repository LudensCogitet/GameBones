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
    GB_INPUT_THRUST,
    GB_INPUT_BREAK,
    GB_INPUT_ROTATE_LEFT,
    GB_INPUT_ROTATE_RIGHT,
    GB_INPUT_SELECT,
    GB_INPUT_BE_AWESOME,
    GB_INPUT_QUIT_GAME,
    GB_INPUT_NUM_INPUTS
} GB_INPUT;

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
