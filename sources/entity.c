#include "../headers/entity.h"
#include "../headers/entities/player_ship.h"
#include "../headers/entities/asteroid.h"

static GbEntity *gb_entity_entities[GB_ENTITY_MAX_ENTITIES];
static unsigned int gb_entity_entities_cursor;

void gb_entity_init() {
    for (unsigned int i = 0; i < GB_ENTITY_MAX_ENTITIES; i++) {
        gb_entity_entities[i] = 0;
    }

    gb_entity_entities_cursor = 0;
}

void gb_entity_teardown() {
    for (unsigned int i = 0; i < gb_entity_entities_cursor; i++) {
        switch (gb_entity_entities[i]->type) {
            case ENTITY_TYPE_PLAYER_SHIP:
                player_ship_destroy((PlayerShip *)gb_entity_entities[i]->entity);
            break;
        }

        free(gb_entity_entities[i]);
        gb_entity_entities[i] = 0;
    }

    gb_entity_entities_cursor = 0;
}

GbEntity *gb_entity_add(GB_ENTITY_TYPE type, void *entity) {
    if (gb_entity_entities_cursor >= GB_ENTITY_MAX_ENTITIES)
        return 0;

    gb_entity_entities[gb_entity_entities_cursor] = (GbEntity *)malloc(sizeof(GbEntity));

    gb_entity_entities[gb_entity_entities_cursor]->type = type;
    gb_entity_entities[gb_entity_entities_cursor]->entity = entity;
    gb_entity_entities[gb_entity_entities_cursor]->dispose = 0;

    return gb_entity_entities[gb_entity_entities_cursor++];
}

void gb_entity_remove(unsigned int entityIndex) {
    switch (gb_entity_entities[entityIndex]->type) {
        case ENTITY_TYPE_PLAYER_SHIP:
            player_ship_destroy((PlayerShip *)gb_entity_entities[entityIndex]->entity);
        break;
    }

    free(gb_entity_entities[entityIndex]);

    if (--gb_entity_entities_cursor > 0) {
        gb_entity_entities[entityIndex] = gb_entity_entities[gb_entity_entities_cursor];
    }

    gb_entity_entities[gb_entity_entities_cursor] = 0;
}

void gb_entity_act(uint32_t delta) {
    for (unsigned int i = 0; i < gb_entity_entities_cursor; i++) {
        if (gb_entity_entities[i]->dispose) {
            gb_entity_remove(i);
        }

        if (gb_entity_entities[i] == 0) break;

        switch (gb_entity_entities[i]->type) {
            case ENTITY_TYPE_PLAYER_SHIP:
                player_ship_act((PlayerShip *)gb_entity_entities[i]->entity, delta);
            break;
            case ENTITY_TYPE_ASTEROID:
                asteroid_act((Asteroid *)gb_entity_entities[i]->entity, delta);
        }
    }
}

