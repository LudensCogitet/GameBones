#ifndef SWITCH_ENTITY_H_INCLUDED
#define SWITCH_ENTITY_H_INCLUDED

#include "../../DynamicEntity/DynamicEntity_type.h"

void switchInit();
void switchTeardown();

DynamicEntity *switchNew(double x, double y);

void switchThink(DynamicEntity *s, double delta);
void switchRespond(DynamicEntity *s, double delta);

#endif // SWITCH_ENTITY_H_INCLUDED
