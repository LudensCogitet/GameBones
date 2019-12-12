#include "../headers/entity.h"
#include "../headers/entities/player_ship.h"
#include "../headers/entities/asteroid.h"
#include "../headers/entities/bullet.h"
#include <stdlib.h>

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
            case ENTITY_TYPE_ASTEROID:
                asteroid_destroy((Asteroid *)gb_entity_entities[i]->entity);
            break;
            case ENTITY_TYPE_BULLET:
                bullet_destroy((Bullet *)gb_entity_entities[i]->entity);
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
    gb_entity_entities[gb_entity_entities_cursor]->messageCursor = 0;
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
        case ENTITY_TYPE_ASTEROID:
            asteroid_destroy((Asteroid *)gb_entity_entities[entityIndex]->entity);
        break;
        case ENTITY_TYPE_BULLET:
            bullet_destroy((Bullet *)gb_entity_entities[entityIndex]->entity);
        break;
    }

    free(gb_entity_entities[entityIndex]);

    if (--gb_entity_entities_cursor > 0) {
        gb_entity_entities[entityIndex] = gb_entity_entities[gb_entity_entities_cursor];
    }

    gb_entity_entities[gb_entity_entities_cursor] = 0;
}

void gb_entity_act(double delta) {
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
            break;
            case ENTITY_TYPE_BULLET:
                bullet_act((Bullet *)gb_entity_entities[i]->entity, delta);
        }
    }
}

void gb_entity_message_send(GbMessage message, GbEntity *entity) {
    if (entity->messageCursor >= GB_ENTITIY_MESSAGE_BOX_SIZE) return;

    entity->messages[entity->messageCursor++] = message;

}

void gb_entity_message_handle() {
    for (unsigned int i = 0; i < gb_entity_entities_cursor; i++) {
        if (!gb_entity_entities[i]->messageCursor) continue;

        switch (gb_entity_entities[i]->type) {
            case ENTITY_TYPE_PLAYER_SHIP:
                player_ship_handle_messages(
                    (PlayerShip *)gb_entity_entities[i]->entity,
                    gb_entity_entities[i]->messages,
                    gb_entity_entities[i]->messageCursor
                );
            break;
            case ENTITY_TYPE_ASTEROID:
                asteroid_handle_messages(
                    (Asteroid *)gb_entity_entities[i]->entity,
                    gb_entity_entities[i]->messages,
                    gb_entity_entities[i]->messageCursor
                );
            break;
            case ENTITY_TYPE_BULLET:
                bullet_handle_messages(
                    (Bullet *)gb_entity_entities[i]->entity,
                    gb_entity_entities[i]->messages,
                    gb_entity_entities[i]->messageCursor
                );
            break;
        }

        gb_entity_entities[i]->messageCursor = 0;
    }
}

