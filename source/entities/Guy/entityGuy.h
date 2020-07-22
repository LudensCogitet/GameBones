#ifndef GUY_ENTITY_H_INCLUDED
#define GUY_ENTITY_H_INCLUDED

#include "SDL2/SDL.h"
#include "../../DynamicEntity/DynamicEntity_type.h"

void guyInit();
void guyTeardown();

DynamicEntity *guyNew(double x, double y);

void guyThink(DynamicEntity *player, double delta);
void guyRespond(DynamicEntity *player, double delta);

#endif // GUY_ENTITY_H_INCLUDED
