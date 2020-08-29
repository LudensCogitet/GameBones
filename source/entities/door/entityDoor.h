#ifndef DOOR_ENTITY_H_INCLUDED
#define DOOR_ENTITY_H_INCLUDED

#include "SDL2/SDL.h"
#include "../../DynamicEntity/DynamicEntity_type.h"

#define DOOR_STATE_OPEN         0x01
#define DOOR_STATE_HORIZONTAL   0x02
#define DOOR_STATE_FORCE_FIELD  0x04

void doorInit();
void doorTeardown();

DynamicEntity *doorNew(double x, double y, uint32_t state);

void doorThink(DynamicEntity *door, double delta);
void doorRespond(DynamicEntity *door, double delta);

#endif // DOOR_ENTITY_H_INCLUDED
