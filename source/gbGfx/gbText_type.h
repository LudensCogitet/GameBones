#ifndef GB_TEXT_H_INCLUDED
#define GB_TEXT_H_INCLUDED

#include "../gbEntity/gbPosition_type.h"
#include "./gbSprite_type.h"

typedef struct {
    gbPosition pos;
    gbSprite *sprite;
} gbText;

#endif // GB_TEXT_H_INCLUDED
