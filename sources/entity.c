#include "../headers/entity.h"
#include "../headers/entities/player_ship.h"
#include "../headers/entities/asteroid.h"
#include "../headers/entities/bullet.h"
#include <stdlib.h>

static GbEntity *gb_entity_entities[ENTITY_PRIORITY_NUM_PRIORITIES][GB_ENTITY_MAX_ENTITIES];
static unsigned int gb_entity_entities_cursors[ENTITY_PRIORITY_NUM_PRIORITIES];

void gb_entity_init() {
    for (unsigned int j = 0; j < ENTITY_PRIORITY_NUM_PRIORITIES; j++) {
        for (unsigned int i = 0; i < GB_ENTITY_MAX_ENTITIES; i++) {
            gb_entity_entities[j][i] = 0;
        }
        gb_entity_entities_cursors[j] = 0;
    }
}

void gb_entity_teardown() {
    for (unsigned int j = 0; j < ENTITY_PRIORITY_NUM_PRIORITIES; j++) {
        for (unsigned int i = 0; i < gb_entity_entities_cursors[j]; i++) {
            switch (gb_entity_entities[j][i]->type) {
                case ENTITY_TYPE_PLAYER_SHIP:
                    player_ship_destroy((PlayerShip *)gb_entity_entities[j][i]->entity);
                break;
                case ENTITY_TYPE_ASTEROID:
                    asteroid_destroy((Asteroid *)gb_entity_entities[j][i]->entity);
                break;
                case ENTITY_TYPE_BULLET:
                    bullet_destroy((Bullet *)gb_entity_entities[j][i]->entity);
                break;
            }

            free(gb_entity_entities[j][i]);
            gb_entity_entities[j][i] = 0;
        }
        gb_entity_entities_cursors[j] = 0;
    }
}

GbEntity *gb_entity_add(GB_ENTITY_TYPE type, void *entity, GB_ENTITY_PRIORITY priority) {
    if (gb_entity_entities_cursors[priority] >= GB_ENTITY_MAX_ENTITIES)
        return 0;

    gb_entity_entities[priority][gb_entity_entities_cursors[priority]] = (GbEntity *)malloc(sizeof(GbEntity));
    gb_entity_entities[priority][gb_entity_entities_cursors[priority]]->messageCursor = 0;
    gb_entity_entities[priority][gb_entity_entities_cursors[priority]]->type = type;
    gb_entity_entities[priority][gb_entity_entities_cursors[priority]]->entity = entity;
    gb_entity_entities[priority][gb_entity_entities_cursors[priority]]->dispose = 0;

    return gb_entity_entities[priority][gb_entity_entities_cursors[priority]++];
}

void gb_entity_remove(GB_ENTITY_PRIORITY priority, unsigned int entityIndex) {
    switch (gb_entity_entities[priority][entityIndex]->type) {
        case ENTITY_TYPE_PLAYER_SHIP:
            player_ship_destroy((PlayerShip *)gb_entity_entities[priority][entityIndex]->entity);
        break;
        case ENTITY_TYPE_ASTEROID:
            asteroid_destroy((Asteroid *)gb_entity_entities[priority][entityIndex]->entity);
        break;
        case ENTITY_TYPE_BULLET:
            bullet_destroy((Bullet *)gb_entity_entities[priority][entityIndex]->entity);
        break;
    }

    free(gb_entity_entities[priority][entityIndex]);

    if (--gb_entity_entities_cursors[priority] > 0) {
        gb_entity_entities[priority][entityIndex] = gb_entity_entities[priority][gb_entity_entities_cursors[priority]];
    }

    gb_entity_entities[priority][gb_entity_entities_cursors[priority]] = 0;
}

void gb_entity_act(double delta) {
    for (unsigned int j = 0; j < ENTITY_PRIORITY_NUM_PRIORITIES; j++) {
        for (unsigned int i = 0; i < gb_entity_entities_cursors[j]; i++) {
            if (gb_entity_entities[j][i]->dispose) {
                gb_entity_remove(j, i);
            }

            if (gb_entity_entities[j][i] == 0) break;

            switch (gb_entity_entities[j][i]->type) {
                case ENTITY_TYPE_PLAYER_SHIP:
                    player_ship_act((PlayerShip *)gb_entity_entities[j][i]->entity, delta);
                break;
                case ENTITY_TYPE_ASTEROID:
                    asteroid_act((Asteroid *)gb_entity_entities[j][i]->entity, delta);
                break;
                case ENTITY_TYPE_BULLET:
                    bullet_act((Bullet *)gb_entity_entities[j][i]->entity, delta);
            }
        }
    }
}

void gb_entity_message_send(GbMessage message, GbEntity *entity) {
    if (entity->messageCursor >= GB_ENTITIY_MESSAGE_BOX_SIZE) return;

    entity->messages[entity->messageCursor++] = message;

}

void gb_entity_message_handle() {
    for (unsigned int j = 0; j < ENTITY_PRIORITY_NUM_PRIORITIES; j++) {
        for (unsigned int i = 0; i < gb_entity_entities_cursors[j]; i++) {
            if (!gb_entity_entities[j][i]->messageCursor) continue;

            switch (gb_entity_entities[j][i]->type) {
                case ENTITY_TYPE_PLAYER_SHIP:
                    player_ship_handle_messages(
                        (PlayerShip *)gb_entity_entities[j][i]->entity,
                        gb_entity_entities[j][i]->messages,
                        gb_entity_entities[j][i]->messageCursor
                    );
                break;
                case ENTITY_TYPE_ASTEROID:
                    asteroid_handle_messages(
                        (Asteroid *)gb_entity_entities[j][i]->entity,
                        gb_entity_entities[j][i]->messages,
                        gb_entity_entities[j][i]->messageCursor
                    );
                break;
                case ENTITY_TYPE_BULLET:
                    bullet_handle_messages(
                        (Bullet *)gb_entity_entities[j][i]->entity,
                        gb_entity_entities[j][i]->messages,
                        gb_entity_entities[j][i]->messageCursor
                    );
                break;
            }

            gb_entity_entities[j][i]->messageCursor = 0;
        }
    }
}

