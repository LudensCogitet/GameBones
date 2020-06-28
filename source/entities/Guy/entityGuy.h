#ifndef GUY_ENTITY_H_INCLUDED
#define GUY_ENTITY_H_INCLUDED

#include "../../gbEntity/gbPosition_type.h"
#include "../../gbGfx/gbSprite/gbSprite_type.h"
#include "../../gbCollision/gbCollisionDynamicRect_type.h"
#include "../../gbAnimation/gbAnimation_type.h"

#include "../../gbEntity/gbEntityType_type.h"
#include "../../gbEntity/gbEntity_sys.h"

typedef enum {
    GUY_STATE_IDLE,
    GUY_STATE_WALK,
    GUY_STATE_NUM_STATES
} GUY_STATE;

typedef struct {
    gbSprite *sprite;
    gbEntity *entity;
    gbCollisionDynamicRect *boundingBox;

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

void initGuy();
gbEntity *guyNew(double x, double y, SDL_RendererFlip flip);
void guyThink(Guy *guy, double delta);
void guyRespond(Guy *guy, gbMessage *messages, uint16_t numMessages);
void guyDispose(Guy *guy);
void guySerialize(Guy * guy, gbFile *file);
Guy *guyDeserialize(gbFile *file);

#endif // GUY_ENTITY_H_INCLUDED
