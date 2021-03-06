#ifndef GB_INPUT_TYPE_H_INCLUDED
#define GB_INPUT_TYPE_H_INCLUDED

typedef enum {
    GB_INPUT_MOVE_LEFT,
    GB_INPUT_MOVE_RIGHT,
    GB_INPUT_JUMP,
    GB_INPUT_INTERACT,
    GB_INPUT_SELECT,
    GB_INPUT_MOUSE_SELECT,
    GB_INPUT_MOUSE_ALT,
    GB_INPUT_TOGGLE_EDIT_MODE,
    GB_INPUT_QUIT_GAME,
    GB_INPUT_NUM_INPUTS
} GB_INPUT;

#endif // GB_INPUT_TYPE_H_INCLUDED
