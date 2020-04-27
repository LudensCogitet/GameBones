#ifndef GB_ENTITY_TYPE_H_INCLUDED
#define GB_ENTITY_TYPE_H_INCLUDED

#define GB_ENTITY_MESSAGE_BOX_SIZE 10

#include "./gbEntityType_type.h"
#include "./gbMessage_type.h"

typedef struct {
    GB_ENTITY_TYPE type;

    uint8_t dispose;

    void *entity;

    gbMessage messages[GB_ENTITY_MESSAGE_BOX_SIZE];
    unsigned int messageCursor;
} gbEntity;

#endif // GB_ENTITY_TYPE_H_INCLUDED
