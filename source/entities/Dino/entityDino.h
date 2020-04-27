#ifndef DINO_ENTITY_H_INCLUDED
#define DINO_ENTITY_H_INCLUDED

#include "../../gbEntity/gbPosition_type.h"
#include "../../gbGfx/gbSprite_type.h"
#include "../../gbAnimation/gbAnimation_type.h"

#include "../../gbEntity/gbEntityType_type.h"
#include "../../gbEntity/gbEntity_sys.h"

typedef struct {
    gbSprite *sprite;
    gbEntity *entity;

    gbPosition pos;
    gbAnimationState animState;

    double dx;
    double dy;
} Dino;

gbEntity *dinoNew(double x, double y);
void dinoThink(Dino *dino, double delta);
void dinoRespond(Dino *dino, gbMessage *messages, uint16_t numMessages);
void dinoDispose(Dino * dino);

#endif // DINO_ENTITY_H_INCLUDED
