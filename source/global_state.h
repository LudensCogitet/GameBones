#ifndef GLOBAL_STATE_H_INCLUDED
#define GLOBAL_STATE_H_INCLUDED

#include "Room/Room_type.h"
#include "DynamicEntity/DynamicEntity_type.h"\

#define MAIN_ROOM_GRID_WIDTH 2
#define MAIN_ROOM_GRID_HEIGHT 2

#define MAIN_ROOM_GRID_MAX_X 1
#define MAIN_ROOM_GRID_MAX_Y 1

extern uint8_t EDIT_MODE;
extern Room *rooms[MAIN_ROOM_GRID_WIDTH][MAIN_ROOM_GRID_HEIGHT];
extern Room *currentRoom;
extern int activeRoomX;
extern int activeRoomY;

extern int mainPowerGridWidth;
extern int mainPowerGridHeight;

extern DynamicEntity *mainPlayer;


#endif // GLOBAL_STATE_H_INCLUDED
