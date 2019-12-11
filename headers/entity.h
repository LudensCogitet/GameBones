#ifndef ENTITY_H_INCLUDED
#define ENTITY_H_INCLUDED

#include <stdint.h>
#include "../headers/message.h"

#define GB_ENTITY_MAX_ENTITIES 1000
#define GB_ENTITIY_MESSAGE_BOX_SIZE 10

typedef struct GbEntity {
    GB_ENTITY_TYPE type;
    uint8_t dispose;
    void *entity;
    GbMessage messages[GB_ENTITIY_MESSAGE_BOX_SIZE];
    unsigned int messageCursor;
} GbEntity;

void gb_entity_init();
void gb_entity_teardown();
GbEntity *gb_entity_add(GB_ENTITY_TYPE type, void *entity);

void gb_entity_message_send(GbMessage message, GbEntity *entity);
void gb_entity_message_handle();

void gb_entity_act(double delta);

#endif
