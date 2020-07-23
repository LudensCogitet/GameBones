#include <stdio.h>
#include <stdlib.h>
#include "./DynamicEntity_sys.h"

#include "./DynamicEntity_type.h"
#include "./DynamicEntityType_enum.h"
#include "./DynamicEntityState_union.h"

#include "../Collision/Collision_sys.h"
#include "../Sprite/Sprite_sys.h"

#include "EntityFunctions.h"

void (*dynamicEntityInits[DYNAMIC_ENTITY_TYPE_NUM_ENTITY_TYPES])();
void (*dynamicEntityTeardowns[DYNAMIC_ENTITY_TYPE_NUM_ENTITY_TYPES])();

DynamicEntity *(*dynamicEntitySetups[DYNAMIC_ENTITY_TYPE_NUM_ENTITY_TYPES])(double x, double y);

void (*dynamicEntityThinks[DYNAMIC_ENTITY_TYPE_NUM_ENTITY_TYPES])(DynamicEntity *entity, double delta);
void (*dynamicEntityResponds[DYNAMIC_ENTITY_TYPE_NUM_ENTITY_TYPES])(DynamicEntity *entity, double delta);

static unsigned int nextId;

static DynamicEntity *entities[DYNAMIC_ENTITY_MAX_ENTITIES];
static unsigned int cursor;

void dynamicEntityInit() {
    nextId = 1;
    cursor = 0;

    for (unsigned int i = 0; i < DYNAMIC_ENTITY_MAX_ENTITIES; i++)
        entities[i] = 0;

    initEntityFunctions();
}

void dynamicEntityTeardown() {
    for (unsigned int i = 0; i < cursor; i++) {
        if(entities[i]) {
            entities[i] = 0;
        }
    }
    cursor = 0;
}

DynamicEntity *dynamicEntityNew(DYNAMIC_ENTITY_TYPE type) {
    DynamicEntity *entity = (DynamicEntity *)malloc(sizeof(DynamicEntity));
    entity->id = nextId++;
    entity->inboxCursor = 0;
    entity->type = type;
    entity->active = 1;

    return entity;
}

int dynamicEntityRegister(DynamicEntity *entity) {
    if (cursor >= DYNAMIC_ENTITY_MAX_ENTITIES) {
        return 0;
    }

    if (nextId < entity->id)
        nextId = entity->id + 1;

    entities[cursor] = entity;
    spriteRegister(&entity->sprite, &entity->pos);
    collisionDynamicRectRegister(&entity->boundingBox, &entity->pos);

    return cursor++;
}

void dynamicEntityUnregister(unsigned int id) {
    unsigned int index = 0;
    for (; index < cursor; index++) {
        if (entities[index] && entities[index]->id == id)
            break;
    }

    if (index == cursor) return;

    spriteUnregister(&entities[index]->sprite);
    collisionDynamicRectUnregister(&entities[index]->boundingBox);

    entities[index] = 0;

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

        dynamicEntityThinks[entities[i]->type](entities[i], delta);
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

        dynamicEntityResponds[entities[i]->type](entities[i], delta);

        entities[i]->inboxCursor = 0;
    }
}

int getEntityCount() {
    return cursor;
}

void dynamicEntitySerializeAll(SDL_RWops *file) {
    if (!cursor) {
        SDL_WriteBE16(file, 0);
        return;
    }

    // Write number of entities
    SDL_WriteBE16(file, cursor);

    // Write each entity's data
    for (int i = 0; i < cursor; i++) {
        DynamicEntity *entity = entities[i];
        SDL_WriteBE16(file, entity->type);
        SDL_WriteBE64(file, entity->pos.x);
        SDL_WriteBE64(file, entity->pos.y);
        SDL_WriteBE32(file, entity->id);
    }
}

DynamicEntity *dynamicEntityDeserialize(SDL_RWops *file) {
    DYNAMIC_ENTITY_TYPE type = SDL_ReadBE16(file);
    double x = SDL_ReadBE64(file);
    double y = SDL_ReadBE64(file);
    uint32_t id = SDL_ReadBE32(file);

    dynamicEntityInits[type]();
    DynamicEntity *entity = dynamicEntitySetups[type](x, y);
    entity->id = id;

    if (nextId <= id) nextId = id + 1;

    return entity;
}
