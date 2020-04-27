#ifndef GB_ENTITY_SYSTEM_H_INCLUDED
#define GB_ENTITY_SYSTEM_H_INCLUDED

#include <stdint.h>
#include "./gbEntity_type.h"
#include "./gbEntityType_type.h"
#include "./gbEntityPriority_type.h"
#include "./gbMessage_type.h"

#define GB_ENTITY_THINK_FUNC void (*)(void *, double)
#define GB_ENTITY_RESPOND_FUNC void (*)(void *, gbMessage *, uint16_t)
#define GB_ENTITY_DISPOSE_FUNC void (*)(void *)

#define GB_ENTITY_MAX_ENTITIES 1000

void (*gbEntityThink[GB_ENTITY_TYPE_NUM_ENTITY_TYPES])(void *entity, double delta);
void (*gbEntityRespond[GB_ENTITY_TYPE_NUM_ENTITY_TYPES])(void *entity, gbMessage *messages, uint16_t numMessages);
void (*gbEntityDispose[GB_ENTITY_TYPE_NUM_ENTITY_TYPES])(void *entity);

void gbEntityInit();
void gbEntityTeardown();
gbEntity *gbEntityNew(GB_ENTITY_TYPE type, void *entity, GB_ENTITY_PRIORITY priority);

void gbEntitySendMessage(gbMessage message, gbEntity *entity);
void gbEntityHandleMessage();

void gbEntityAct(double delta);

#endif // GB_ENTITY_SYSTEM_H_INCLUDED
