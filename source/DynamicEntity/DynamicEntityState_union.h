#ifndef DYNAMIC_ENTITY_STATE_H_INCLUDED
#define DYNAMIC_ENTITY_STATE_H_INCLUDED

typedef enum {
    GUY_STATE_IDLE,
    GUY_STATE_WALK,
    GUY_STATE_NUM_STATES
} GUY_STATE;

typedef struct {
    GUY_STATE state;
    int direction;
    unsigned int moveKeysDown;
} GuyState;

typedef enum {
    MOVE_ROOM_PANEL_STATE_EMPTY,
    MOVE_ROOM_PANEL_STATE_DOWN,
    MOVE_ROOM_PANEL_STATE_UP,
    MOVE_ROOM_PANEL_STATE_RIGHT,
    MOVE_ROOM_PANEL_STATE_LEFT,
    MOVE_ROOM_PANEL_STATE_NUM_STATES
} MOVE_ROOM_PANEL_STATE;

typedef struct {
    MOVE_ROOM_PANEL_STATE state;
} MoveRoomPanelState;

typedef union {
    GuyState guy;
    MoveRoomPanelState moveRoomPanel;
} DynamicEntityState;

#endif // DYNAMIC_ENTITY_STATE_H_INCLUDED
