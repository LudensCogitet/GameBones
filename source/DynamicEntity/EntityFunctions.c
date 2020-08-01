#include "DynamicEntityType_enum.h"
#include "DynamicEntity_sys.h"

#include "../entities/Guy/entityGuy.h"
#include "../entities/MoveRoomPanel/entityMoveRoomPanel.h"

void initEntityFunctions() {
    // Guy
    dynamicEntityInits[DYNAMIC_ENTITY_TYPE_GUY]     = &guyInit;
    dynamicEntityTeardowns[DYNAMIC_ENTITY_TYPE_GUY] = &guyTeardown;
    dynamicEntitySetups[DYNAMIC_ENTITY_TYPE_GUY]    = &guyNew;
    dynamicEntityThinks[DYNAMIC_ENTITY_TYPE_GUY]    = &guyThink;
    dynamicEntityResponds[DYNAMIC_ENTITY_TYPE_GUY]  = &guyRespond;

    // Move Room Panel
    dynamicEntityInits[DYNAMIC_ENTITY_TYPE_MOVE_ROOM_PANEL]     = &moveRoomPanelInit;
    dynamicEntityTeardowns[DYNAMIC_ENTITY_TYPE_MOVE_ROOM_PANEL] = &moveRoomPanelTeardown;
    dynamicEntitySetups[DYNAMIC_ENTITY_TYPE_MOVE_ROOM_PANEL]    = &moveRoomPanelNew;
    dynamicEntityThinks[DYNAMIC_ENTITY_TYPE_MOVE_ROOM_PANEL]    = &moveRoomPanelThink;
    dynamicEntityResponds[DYNAMIC_ENTITY_TYPE_MOVE_ROOM_PANEL]  = &moveRoomPanelRespond;
}
