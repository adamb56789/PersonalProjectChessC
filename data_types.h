#ifndef DATA_TYPES_H
#define DATA_TYPES_H

#include <stdbool.h>

enum player
{
    white,
    black
};

typedef struct
{
    enum player turn;
    bool left_r_stationary;
    bool k_stationary;
    bool right_r_stationary;
    bool left_R_stationary;
    bool K_stationary;
    bool right_R_stationary;
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
