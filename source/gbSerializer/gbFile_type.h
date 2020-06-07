#ifndef GB_FILE_H_INCLUDED
#define GB_FILE_H_INCLUDED

#include "SDL2/SDL.h"

typedef enum {
    GB_ENDIAN_BIG,
    GB_ENDIAN_LITTLE
} GB_ENDIAN;

typedef struct {
    GB_ENDIAN endianness;
    SDL_RWops *file;
} gbFile;

#endif // GB_FILE_H_INCLUDED
