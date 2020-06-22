#ifndef GB_ENTITY_SYSTEM_H_INCLUDED
#define GB_ENTITY_SYSTEM_H_INCLUDED

#include <stdint.h>
#include "./gbEntity_type.h"
#include "./gbEntityType_type.h"
#include "./gbEntityPriority_type.h"
#include "./gbMessage_type.h"
#include "../gbSerializer/gbFile_type.h"

#define GB_ENTITY_THINK_FUNC_PARAM(x) void (*x)(void *, double)
#define GB_ENTITY_THINK_FUNC void (*)(void *, double)

#define GB_ENTITY_RESPOND_FUNC_PARAM(x) void (*x)(void *, gbMessage *, uint16_t)
#define GB_ENTITY_RESPOND_FUNC void (*)(void *, gbMessage *, uint16_t)

#define GB_ENTITY_DISPOSE_FUNC_PARAM(x) void (*x)(void *)
#define GB_ENTITY_DISPOSE_FUNC void (*)(void *)

// TODO: Need to change this signature for actual functionality.
#define GB_ENTITY_SERIALIZE_FUNC_PARAM(x) void (*x)(void *, gbFile *)
#define GB_ENTITY_SERIALIZE_FUNC void (*)(void *, gbFile *)

#define GB_ENTITY_DESERIALIZE_FUNC_PARAM(x) void *(*x)(gbFile *)
#define GB_ENTITY_DESERIALIZE_FUNC void *(*)(gbFile *)

#define GB_ENTITY_MAX_ENTITIES 1000

void (*gbEntityThink[GB_ENTITY_TYPE_NUM_ENTITY_TYPES])(void *entity, double delta);
void (*gbEntityRespond[GB_ENTITY_TYPE_NUM_ENTITY_TYPES])(void *entity, gbMessage *messages, uint16_t numMessages);
void (*gbEntityDispose[GB_ENTITY_TYPE_NUM_ENTITY_TYPES])(void *entity);

// TODO: Need to change this signature for actual functionality.
void (*gbEntitySerialize[GB_ENTITY_TYPE_NUM_ENTITY_TYPES])(void *, gbFile *);
void *(*gbEntityDeserialize[GB_ENTITY_TYPE_NUM_ENTITY_TYPES])(gbFile *);

void gbEntityInit();
void gbEntityTeardown();
gbEntity *gbEntityNew(GB_ENTITY_TYPE type, void *entity, GB_ENTITY_PRIORITY priority);
gbEntity *gbEntityFindOfType(GB_ENTITY_TYPE type);

void gbEntitySendMessage(gbMessage message, gbEntity *entity);
void gbEntityHandleMessage();

void gbEntityAct(double delta);

void gbEntityRegisterFuncs(
                           GB_ENTITY_TYPE type,
                           GB_ENTITY_THINK_FUNC_PARAM(think),
                           GB_ENTITY_RESPOND_FUNC_PARAM(respond),
                           GB_ENTITY_DISPOSE_FUNC_PARAM(dispose),
                           GB_ENTITY_SERIALIZE_FUNC_PARAM(serialize),
                           GB_ENTITY_DESERIALIZE_FUNC_PARAM(deserialize)
                           );

#endif // GB_ENTITY_SYSTEM_H_INCLUDED
