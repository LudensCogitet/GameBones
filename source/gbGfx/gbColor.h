#ifndef GB_COLOR_H_INCLUDED
#define GB_COLOR_H_INCLUDED

#include "SDL2/SDL.h"

typedef enum {
    GB_COLOR_WHITE,
    GB_COLOR_BLACK,
    GB_COLOR_NUM_COLORS
} GB_COLOR;

extern SDL_Color colors[GB_COLOR_NUM_COLORS];
#endif // GB_COLOR_H_INCLUDED
