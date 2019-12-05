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
    GbCollision collision;
} GbMessageCollision;

typedef union GbMessage {
    GB_MESSAGE_TYPE type;
    GbMessageCollision collision;
} GbMessage;

#endif
