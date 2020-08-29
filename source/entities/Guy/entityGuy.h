#ifndef GUY_ENTITY_H_INCLUDED
#define GUY_ENTITY_H_INCLUDED

#include "SDL2/SDL.h"
#include "../../DynamicEntity/DynamicEntity_type.h"

#define GUY_STATE_IDLE 0
#define GUY_STATE_WALK 1

void guyInit();
void guyTeardown();

DynamicEntity *guyNew(double x, double y, uint32_t state);

void guyThink(DynamicEntity *guy, double delta);
void guyRespond(DynamicEntity *guy, double delta);

#endif // GUY_ENTITY_H_INCLUDED
