#ifndef GB_FONT_H_INCLUDED
#define GB_FONT_H_INCLUDED

#include "SDL2/SDL_ttf.h"

typedef enum {
    GB_FONT_MID_FREE_MONO,
    GB_FONT_NUM_FONTS
} GB_FONT;

extern TTF_Font *fonts[GB_FONT_NUM_FONTS];

#endif // GB_FONT_H_INCLUDED
