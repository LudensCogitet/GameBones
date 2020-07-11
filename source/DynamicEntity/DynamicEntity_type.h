#ifndef DYNAMIC_ENTITY_H_INCLUDED
#define DYNAMIC_ENTITY_H_INCLUDED

#include "../Position_type.h"
#include "../Sprite/Sprite_type.h"
#include "../Collision/CollisionDynamicRect_type.h"
#include "../gbAnimation/gbAnimation_type.h"

#include "DynamicEntityType_enum.h"
#include "DynamicEntityState_union.h"
#include "Message_type.h"

#define DYNAMIC_ENTITY_INBOX_SIZE 5

typedef struct {
    unsigned int id;
    DYNAMIC_ENTITY_TYPE type;

    uint8_t active;

    DynamicEntityState state;

    Position pos;

    Sprite sprite;
    CollisionDynamicRect boundingBox;

    gbAnimationState animState;

    unsigned int inboxCursor;
    Message inbox[DYNAMIC_ENTITY_INBOX_SIZE];

    double dx;
    double dy;

    double ax;
    double ay;
} DynamicEntity;

#endif // DYNAMIC_ENTITY_H_INCLUDED
