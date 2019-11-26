#ifndef INCLUDED_MESSAGE_H
#define INCLUDED_MESSAGE_H

#include "../headers/physics.h"

typedef enum GB_MESSAGE_TYPE {
    MESSAGE_COLLISION,
    MESSAGE_NUM_MESSAGE_TYPES
} GB_MESSAGE_TYPE;


typedef struct GbEntity GbEntity;
typedef struct GbMessageCollision {
    GB_MESSAGE_TYPE type;
    double x;
    double y;
    float dx;
    float dy;

    double m;

    GB_ENTITY_TYPE other_type;
    GbCollider other_collider;
} GbMessageCollision;

typedef union GbMessage {
    GB_MESSAGE_TYPE type;
    GbMessageCollision collision;
} GbMessage;

#endif
