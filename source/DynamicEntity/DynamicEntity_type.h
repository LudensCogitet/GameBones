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
    uint32_t id;
    DYNAMIC_ENTITY_TYPE type;

    _Bool active;

    uint32_t state;

    Position pos;

    Sprite sprite;
    CollisionDynamicRect boundingBox;

    gbAnimationState animState;

    uint8_t inboxCursor;
    Message inbox[DYNAMIC_ENTITY_INBOX_SIZE];

    double dx;
    double dy;

    double ax;
    double ay;
} DynamicEntity;

#endif // DYNAMIC_ENTITY_H_INCLUDED
