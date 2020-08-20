#ifndef DOOR_ENTITY_H_INCLUDED
#define DOOR_ENTITY_H_INCLUDED

#include "SDL2/SDL.h"
#include "../../DynamicEntity/DynamicEntity_type.h"

void doorInit();
void doorTeardown();

DynamicEntity *doorNew(double x, double y);

void doorThink(DynamicEntity *door, double delta);
void doorRespond(DynamicEntity *door, double delta);

#endif // DOOR_ENTITY_H_INCLUDED
