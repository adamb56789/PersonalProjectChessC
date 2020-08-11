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
    char turn;
    int public_depth;
} state_t;

typedef struct
{
    char piece;
    char from_y;
    char from_x;
    char to_y;
    char to_x;
    char target;
} move_t;

typedef struct
{
    char a;
    char b;
} pair;

#endif
