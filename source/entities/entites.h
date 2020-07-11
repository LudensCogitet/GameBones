#ifndef ENTITIES_H_INCLUDED
#define ENTITIES_H_INCLUDED

#include "../gbGfx/gbSprite/gbSprite_type.h"
#include "../gbCollision/gbCollisionDynamicRect_type.h"
#include "../gbAnimation/gbAnimation_type.h"

typedef struct {
    ENTITY_TYPE type;
    uint8_t dispose;
    unsigned int refCount;

    gbMessage messages[GB_ENTITY_MESSAGE_BOX_SIZE];
    unsigned int messageCursor;
} EntityInfo;

// GUY
typedef enum {
    GUY_STATE_IDLE,
    GUY_STATE_WALK,
    GUY_STATE_NUM_STATES
} GUY_STATE;

typedef struct Guy {
    EntityInfo info;

    gbSprite sprite;
    gbCollisionDynamicRect boundingBox;

    gbPosition pos;
    gbAnimationState animState;

    GUY_STATE state;

    double dx;
    double dy;

    double ax;
    double ay;

    int direction;
    unsigned int moveKeysDown;
} Guy;
// END GUY

typedef union Entity {
   EntityInfo info; // All entity classes must implement
   Guy guy;
} Entity;

#endif // ENTITIES_H_INCLUDED
