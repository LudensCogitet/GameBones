#ifndef DYNAMIC_ENTITY_SYSTEM_H_INCLUDED
#define DYNAMIC_ENTITY_SYSTEM_H_INCLUDED

#include <stdint.h>
#include "DynamicEntity_type.h"
#include "DynamicEntityType_enum.h"
#include "Message_type.h"

#define DYNAMIC_ENTITY_THINK_FUNC_PARAM(x) void (*x)(DynamicEntity *, double)
#define DYNAMIC_ENTITY_THINK_FUNC void (*)(DynamicEntity *, double)

#define DYNAMIC_ENTITY_RESPOND_FUNC_PARAM(x) void (*x)(DynamicEntity *, double)
#define DYNAMIC_ENTITY_RESPOND_FUNC void (*)(DynamicEntity *, double)

#define DYNAMIC_ENTITY_MAX_MESSAGES 5

#define DYNAMIC_ENTITY_MAX_ENTITIES 1000

extern void (*dynamicEntityThink[DYNAMIC_ENTITY_TYPE_NUM_ENTITY_TYPES])(DynamicEntity *entity, double delta);
extern void (*dynamicEntityRespond[DYNAMIC_ENTITY_TYPE_NUM_ENTITY_TYPES])(DynamicEntity *entity, double delta);

void dynamicEntityInit();
void dynamicEntityTeardown();
void dynamicEntityAct(double delta);
void dynamicEntityHandleMessages(double delta);

DynamicEntity *dynamicEntityNew(DYNAMIC_ENTITY_TYPE type);
void dynamicEntityRemove(unsigned int id);

DynamicEntity *dynamicEntityFindById(unsigned int id);
DynamicEntity *dynamicEntityFindOfType(DYNAMIC_ENTITY_TYPE type);

void dynamicEntitySendMessage(Message message, unsigned int entityId);

void dynamicEntityRegisterFuncs(
                           DYNAMIC_ENTITY_TYPE type,
                           DYNAMIC_ENTITY_THINK_FUNC_PARAM(think),
                           DYNAMIC_ENTITY_RESPOND_FUNC_PARAM(respond)
                           );

#endif // DYNAMIC_ENTITY_SYSTEM_H_INCLUDED
