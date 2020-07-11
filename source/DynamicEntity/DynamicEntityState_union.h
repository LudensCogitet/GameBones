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

typedef union {
    GuyState guy;
} DynamicEntityState;

#endif // DYNAMIC_ENTITY_STATE_H_INCLUDED
