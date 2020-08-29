#ifndef MOVE_ROOM_PANEL_ENTITY_H_INCLUDED
#define MOVE_ROOM_PANEL_ENTITY_H_INCLUDED

#include "../../DynamicEntity/DynamicEntity_type.h"

#define MOVE_ROOM_PANEL_STATE_EMPTY     0
#define MOVE_ROOM_PANEL_STATE_DOWN      1
#define MOVE_ROOM_PANEL_STATE_UP        2
#define MOVE_ROOM_PANEL_STATE_RIGHT     3
#define MOVE_ROOM_PANEL_STATE_LEFT      4
#define MOVE_ROOM_PANEL_STATE_NO_STATE  5

void moveRoomPanelInit();
void moveRoomPanelTeardown();

DynamicEntity *moveRoomPanelNew(double x, double y, uint32_t state);

void moveRoomPanelThink(DynamicEntity *panel, double delta);
void moveRoomPanelRespond(DynamicEntity *panel, double delta);

#endif // MOVE_ROOM_PANEL_ENTITY_H_INCLUDED
