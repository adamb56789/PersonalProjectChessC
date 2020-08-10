#ifndef DATA_TYPES_H
#define DATA_TYPES_H


#include <stdbool.h>

typedef struct
{
    bool move;
    bool qcastle;
    bool kcastle;
    bool Qcastle;
    bool Kcastle;
    bool canEP;
    int public_depth;
} state_t;

typedef struct {
    char piece;
    int from;
    int to;
    char target;
} move_t;

#endif
