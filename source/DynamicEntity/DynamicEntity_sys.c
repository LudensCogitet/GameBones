#include <stdio.h>
#include <stdlib.h>
#include "./DynamicEntity_sys.h"

#include "./DynamicEntity_type.h"
#include "./DynamicEntityType_enum.h"
#include "./DynamicEntityState_union.h"

void (*dynamicEntityThink[DYNAMIC_ENTITY_TYPE_NUM_ENTITY_TYPES])(DynamicEntity *entity, double delta);
void (*dynamicEntityRespond[DYNAMIC_ENTITY_TYPE_NUM_ENTITY_TYPES])(DynamicEntity *entity, double delta);

static unsigned int nextId;

static DynamicEntity *entities[DYNAMIC_ENTITY_MAX_ENTITIES];
static unsigned int cursor;

void dynamicEntityInit() {
    nextId = 1;
    cursor = 0;

    for (unsigned int i = 0; i < DYNAMIC_ENTITY_MAX_ENTITIES; i++)
        entities[i] = 0;


    for (unsigned int i = 0; i < DYNAMIC_ENTITY_TYPE_NUM_ENTITY_TYPES; i++) {
        dynamicEntityThink[i] = 0;
        dynamicEntityRespond[i] = 0;
    }
}

void dynamicEntityTeardown() {
    for (unsigned int i = 0; i < cursor; i++) {
        if(entities[i]) {
            free(entities[i]);
            entities[i] = 0;
        }
    }
    cursor = 0;
}

DynamicEntity *dynamicEntityNew(DYNAMIC_ENTITY_TYPE type) {
    if (cursor >= DYNAMIC_ENTITY_MAX_ENTITIES) {
        return 0;
    }

    DynamicEntity *entity = (DynamicEntity *)malloc(sizeof(DynamicEntity));
    entity->id = nextId++;
    entity->inboxCursor = 0;
    entity->type = type;
    entity->active = 1;

    entities[cursor++] = entity;

    return entity;
}


void dynamicEntityRemove(unsigned int id) {
    unsigned int index = 0;
    for (; index < cursor; index++) {
        if (entities[index] && entities[index]->id == id) {
            free(entities[index]);
        }
    }

    if (--cursor > 0) {
        entities[index] = entities[cursor];
    }

    entities[cursor] = 0;
}

DynamicEntity *dynamicEntityFindById(unsigned int id) {
    for (unsigned int i = 0; i < cursor; i++) {
        if (entities[i]->id == id)
            return entities[i];
    }

    return 0;
}

DynamicEntity *dynamicEntityFindOfType(DYNAMIC_ENTITY_TYPE type) {
    for (unsigned int i = 0; i < cursor; i++) {
        if (entities[i]->type == type) {
            return entities[i];
        }
    }

    return 0;
}

void dynamicEntityAct(double delta) {
    for (unsigned int i = 0; i < cursor; i++) {
        if (!entities[i] || !entities[i]->active) continue;

        dynamicEntityThink[entities[i]->type](entities[i], delta);
    }
}

void dynamicEntitySendMessage(Message message, unsigned int entityId) {
    DynamicEntity *entity = dynamicEntityFindById(entityId);

    if (!entity) return;

    if (entity->inboxCursor >= DYNAMIC_ENTITY_MAX_MESSAGES) return;

    entity->inbox[entity->inboxCursor++] = message;
}

void dynamicEntityHandleMessages(double delta) {
    for (unsigned int i = 0; i < cursor; i++) {
        if (!entities[i]->inboxCursor) continue;

        dynamicEntityRespond[entities[i]->type](entities[i], delta);

        entities[i]->inboxCursor = 0;
    }
}

void dynamicEntityRegisterFuncs(
                           DYNAMIC_ENTITY_TYPE type,
                           DYNAMIC_ENTITY_THINK_FUNC_PARAM(think),
                           DYNAMIC_ENTITY_RESPOND_FUNC_PARAM(respond)
                           ) {
    dynamicEntityThink[type] = think;
    dynamicEntityRespond[type] = respond;
}
