#ifndef INPUT_H_INCLUDED
#define INPUT_H_INCLUDED

#include <stdint.h>
#include <SDL2/SDL.h>
#include "./gbInput_type.h"
#include "./gbInputState_type.h"

#define GB_INPUT_PRESSED          1
#define GB_INPUT_JUST_PRESSED     2
#define GB_INPUT_RELEASED         4
#define GB_INPUT_SHIFT            8
#define GB_INPUT_ALT             16
#define GB_INPUT_CTRL            32
#define GB_INPUT_DOUBLE_PRESSED  64

#define GB_INPUT_BUFFER_MAX_LEN 201

#define GB_INPUT_BUFFER_READ    0
#define GB_INPUT_BUFFER_UNREAD  1
#define GB_INPUT_BUFFER_STOP    2

int gbInputInit();
void gbInputUpdate();
void gbInputTeardown();

GbInputState gbInputGetState(GB_INPUT input);
uint8_t gbInputCheckState(GB_INPUT input, uint8_t mask);
void gbInputSetKey(GB_INPUT input, uint32_t key);
void gbInputClearState(GB_INPUT input);

void gbInputStartTextInput();
void gbInputClearTextInput();
void gbInputStopTextInput();
uint8_t gbInputReadTextInput(char *buffer);

//void gb_input_get_mouse_pos_screen(int *x, int *y);
//void gb_input_get_mouse_pos_world(int *x, int *y);

#endif
