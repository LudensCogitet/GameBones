#ifndef MESSAGE_H_INCLUDED
#define MESSAGE_H_INCLUDED

typedef enum {
    MESSAGE_COLLISION,
    MESSAGE_NUM_MESSAGE_TYPES
} MESSAGE_TYPE;

typedef struct {
    MESSAGE_TYPE type;
    unsigned int senderId;
} Message;

#endif //MESSAGE_H_INCLUDED
