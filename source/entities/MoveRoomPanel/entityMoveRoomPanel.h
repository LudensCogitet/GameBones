#ifndef MOVE_ROOM_PANEL_ENTITY_H_INCLUDED
#define MOVE_ROOM_PANEL_ENTITY_H_INCLUDED

#include "../../DynamicEntity/DynamicEntity_type.h"

void moveRoomPanelInit();
void moveRoomPanelTeardown();

DynamicEntity *moveRoomPanelNew(double x, double y);

void moveRoomPanelThink(DynamicEntity *panel, double delta);
void moveRoomPanelRespond(DynamicEntity *panel, double delta);

#endif // MOVE_ROOM_PANEL_ENTITY_H_INCLUDED
