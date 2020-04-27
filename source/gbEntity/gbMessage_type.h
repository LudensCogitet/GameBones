#ifndef GB_MESSAGE_TYPE_H_INCLUDED
#define GB_MESSAGE_TYPE_H_INCLUDED

typedef enum {
    MESSAGE_COLLISION,
    MESSAGE_NUM_MESSAGE_TYPES
} GB_MESSAGE_TYPE;

typedef struct {
    GB_MESSAGE_TYPE type;
} gbMessageCollision;

typedef union {
    GB_MESSAGE_TYPE type;
} gbMessage;

#endif //GB_MESSAGE_TYPE_H_INCLUDED
