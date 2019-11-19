#ifndef ENTITY_H_INCLUDED
#define ENTITY_H_INCLUDED

#include "./entities/player_ship.h"

#define GB_ENTITY_MAX_ENTITIES 100

typedef enum GB_ENTITY_TYPE {
    ENTITY_TYPE_PLAYER_SHIP,
    ENTITY_TYPE_ASTEROID,
    ENTITY_TYPE_NUM_ENTITY_TYPES
} GB_ENTITY_TYPE;

typedef struct GbEntity {
    GB_ENTITY_TYPE type;
    uint8_t dispose;
    void *entity;
} GbEntity;

void gb_entity_init();
void gb_entity_teardown();
GbEntity *gb_entity_add(GB_ENTITY_TYPE type, void *entity);
void gb_entity_act(uint32_t delta);

#endif
