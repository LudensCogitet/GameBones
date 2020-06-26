#include <stdio.h>
#include <stdlib.h>
#include "./gbEntity_sys.h"

#include "./gbEntity_type.h"
#include "./gbEntityPriority_type.h"
#include "./gbEntityType_type.h"

static gbEntity *entities[GB_ENTITY_PRIORITY_NUM_PRIORITIES][GB_ENTITY_MAX_ENTITIES];
static unsigned int entitiesCursors[GB_ENTITY_PRIORITY_NUM_PRIORITIES];

void gbEntityInit() {
    for (unsigned int j = 0; j < GB_ENTITY_PRIORITY_NUM_PRIORITIES; j++) {
        for (unsigned int i = 0; i < GB_ENTITY_MAX_ENTITIES; i++) {
            entities[j][i] = 0;
        }
        entitiesCursors[j] = 0;
    }

    for (unsigned int i = 0; i < GB_ENTITY_TYPE_NUM_ENTITY_TYPES; i++) {
        gbEntityThink[i] = 0;
        gbEntityRespond[i] = 0;
        gbEntityDispose[i] = 0;
    }
}

void gbEntityTeardown() {
    for (unsigned int j = 0; j < GB_ENTITY_PRIORITY_NUM_PRIORITIES; j++) {
        for (unsigned int i = 0; i < entitiesCursors[j]; i++) {
            gbEntityDispose[entities[j][i]->type](entities[j][i]->entity);

            free(entities[j][i]);
            entities[j][i] = 0;
        }
        entitiesCursors[j] = 0;
    }
}

gbEntity *gbEntityNew(GB_ENTITY_TYPE type, void *entity, GB_ENTITY_PRIORITY priority) {
    unsigned int index = entitiesCursors[priority];
    if (index >= GB_ENTITY_MAX_ENTITIES)
        for (index = 0; index <= GB_ENTITY_MAX_ENTITIES; index++) {
            if (entities[priority][index] == 0) break;
        }

    if (index == GB_ENTITY_MAX_ENTITIES) {
        fprintf(stderr, "Failed to generate entity: entity limit reached");
        return 0;
    }

    entities[priority][index] = (gbEntity *)malloc(sizeof(gbEntity));
    entities[priority][index]->messageCursor = 0;
    entities[priority][index]->type = type;
    entities[priority][index]->entity = entity;
    entities[priority][index]->dispose = 0;

    if (index == entitiesCursors[priority]) {
        entitiesCursors[priority]++;
    }

    return entities[priority][index];
}

gbEntity *gbEntityFindOfType(GB_ENTITY_TYPE type) {
    for (unsigned int i = 0; i < GB_ENTITY_PRIORITY_NUM_PRIORITIES; i++) {
        for (unsigned int j = 0; j < entitiesCursors[i]; j++) {
            if (!entities[i][j]) continue;

            if (entities[i][j]->type == type) {
                return entities[i][j];
            }
        }
    }

    return 0;
}

void gbEntityDestroy(GB_ENTITY_PRIORITY priority, unsigned int entityIndex) {
    gbEntityDispose[entities[priority][entityIndex]->type](entities[priority][entityIndex]->entity);

    free(entities[priority][entityIndex]);

    entities[priority][entityIndex] = 0;
}

void gbEntityAct(double delta) {
    for (unsigned int j = 0; j < GB_ENTITY_PRIORITY_NUM_PRIORITIES; j++) {
        for (unsigned int i = 0; i < entitiesCursors[j]; i++) {
            if (!entities[j][i]) continue;

            if (entities[j][i]->dispose) {
                gbEntityDestroy(j, i);
                continue;
            }

            gbEntityThink[entities[j][i]->type](entities[j][i]->entity, delta);
        }
    }
}

void gbEntitySendMessage(gbMessage message, gbEntity *entity) {
    if (entity->messageCursor >= GB_ENTITY_MESSAGE_BOX_SIZE) return;

    entity->messages[entity->messageCursor++] = message;
}

void gbEntityHandleMessage() {
    for (unsigned int j = 0; j < GB_ENTITY_PRIORITY_NUM_PRIORITIES; j++) {
        for (unsigned int i = 0; i < entitiesCursors[j]; i++) {
            if (!entities[j][i]->messageCursor) continue;

            gbEntityRespond[entities[j][i]->type](
                        entities[j][i]->entity,
                        entities[j][i]->messages,
                        entities[j][i]->messageCursor
                        );

            entities[j][i]->messageCursor = 0;
        }
    }
}

void gbEntityRegisterFuncs(
                           GB_ENTITY_TYPE type,
                           GB_ENTITY_THINK_FUNC_PARAM(think),
                           GB_ENTITY_RESPOND_FUNC_PARAM(respond),
                           GB_ENTITY_DISPOSE_FUNC_PARAM(dispose),
                           GB_ENTITY_SERIALIZE_FUNC_PARAM(serialize),
                           GB_ENTITY_DESERIALIZE_FUNC_PARAM(deserialize)
                           ) {
    gbEntityThink[type] = think;
    gbEntityRespond[type] = respond;
    gbEntityDispose[type] = dispose;
    gbEntitySerialize[type] = serialize;
    gbEntityDeserialize[type] = deserialize;
}
