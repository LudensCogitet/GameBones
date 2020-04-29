#ifndef DINO_ENTITY_H_INCLUDED
#define DINO_ENTITY_H_INCLUDED

#include "../../gbEntity/gbPosition_type.h"
#include "../../gbGfx/gbSprite_type.h"
#include "../../gbAnimation/gbAnimation_type.h"

#include "../../gbEntity/gbEntityType_type.h"
#include "../../gbEntity/gbEntity_sys.h"

typedef enum {
    DINO_STATE_IDLE,
    DINO_STATE_SPEED_UP,
    DINO_STATE_SLOW_DOWN,
    DINO_STATE_RUN,
    DINO_STATE_NUM_STATES
} DINO_STATE;

typedef struct {
    gbSprite *sprite;
    gbEntity *entity;

    gbPosition pos;
    gbAnimationState animState;

    DINO_STATE state;

    double velocity;
    double acceleration;
    int direction;
} Dino;

gbEntity *dinoNew(double x, double y, SDL_RendererFlip flip);
void dinoThink(Dino *dino, double delta);
void dinoRespond(Dino *dino, gbMessage *messages, uint16_t numMessages);
void dinoDispose(Dino * dino);

#endif // DINO_ENTITY_H_INCLUDED
