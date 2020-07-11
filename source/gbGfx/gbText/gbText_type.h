#ifndef GB_TEXT_H_INCLUDED
#define GB_TEXT_H_INCLUDED

#include "../../gbEntity/gbPosition_type.h"
#include "../gbSprite/gbSprite_type.h"

#include "../gbColor.h"
#include "../gbFont.h"

typedef struct {
    gbPosition pos;
    gbSprite *sprite;
} gbText;

gbText *gbTextNew(const char *text,
                       GB_FONT font,
                       GB_COLOR color,
                       GB_SPRITE_LAYER layer,
                       double x,
                       double y,
                       uint8_t active,
                       uint8_t fixed);
void gbTextDelete(gbText *textGraphic);
void gbTextChange(gbText *textGraphic, GB_FONT font, GB_COLOR color, const char *text);

#endif // GB_TEXT_H_INCLUDED
