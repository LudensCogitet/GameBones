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
    if (entitiesCursors[priority] >= GB_ENTITY_MAX_ENTITIES)
        return 0;

    entities[priority][entitiesCursors[priority]] = (gbEntity *)malloc(sizeof(gbEntity));
    entities[priority][entitiesCursors[priority]]->messageCursor = 0;
    entities[priority][entitiesCursors[priority]]->type = type;
    entities[priority][entitiesCursors[priority]]->entity = entity;
    entities[priority][entitiesCursors[priority]]->dispose = 0;

    return entities[priority][entitiesCursors[priority]++];
}

void gbEntityDestroy(GB_ENTITY_PRIORITY priority, unsigned int entityIndex) {
    gbEntityDispose[entities[priority][entityIndex]->type](entities[priority][entityIndex]->entity);

    free(entities[priority][entityIndex]);

    if (--entitiesCursors[priority] > 0) {
        entities[priority][entityIndex] = entities[priority][entitiesCursors[priority]];
    }

    entities[priority][entitiesCursors[priority]] = 0;
}

void gbEntityAct(double delta) {
    for (unsigned int j = 0; j < GB_ENTITY_PRIORITY_NUM_PRIORITIES; j++) {
        for (unsigned int i = 0; i < entitiesCursors[j]; i++) {
            if (entities[j][i]->dispose) {
                gbEntityDestroy(j, i);
            }

            if (entities[j][i] == 0) break;

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

