#ifndef GUY_ENTITY_H_INCLUDED
#define GUY_ENTITY_H_INCLUDED

#include "SDL2/SDL.h"
#include "../../DynamicEntity/DynamicEntity_type.h"

void initGuy();
DynamicEntity *guyNew(double x, double y, SDL_RendererFlip flip);
void guyThink(DynamicEntity *guy, double delta);
void guyRespond(DynamicEntity *guy, double delta);

#endif // GUY_ENTITY_H_INCLUDED
