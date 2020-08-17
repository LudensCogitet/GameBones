#include "global_state.h"

uint8_t EDIT_MODE = 0;
Room *rooms [MAIN_ROOM_GRID_WIDTH][MAIN_ROOM_GRID_HEIGHT] = { 0 };
Room *currentRoom = 0;

int mainPowerGridWidth;
int mainPowerGridHeight;

int activeRoomX = 0;
int activeRoomY = 0;
DynamicEntity *mainPlayer = 0;
