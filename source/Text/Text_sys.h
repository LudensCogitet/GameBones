#ifndef TEXT_SYS_H_INCLUDED
#define TEXT_SYS_H_INCLUDED

#include "Text_type.h"
#include "../Sprite/SpriteLayer_enum.h"

#include "../gbGfx/gbColor.h"
#include "../gbGfx/gbFont.h"

void textNew(Text *textStruct,
               const char *text,
               GB_FONT font,
               GB_COLOR color,
               SPRITE_LAYER layer,
               double x,
               double y,
               uint8_t active,
               uint8_t fixed);
void textDelete(Text *textGraphic);
void textChange(Text *textGraphic, GB_FONT font, GB_COLOR color, const char *text);


#endif // TEXT_SYS_H_INCLUDED
