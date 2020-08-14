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
    bool queenside_r_stationary;
    bool k_stationary;
    bool kingside_r_stationary;
    bool queenside_R_stationary;
    bool K_stationary;
    bool kingside_R_stationary;
    char ep_x;
} state_t;

typedef struct
{
    char piece;
    char from_y;
    char from_x;
    char to_y;
    char to_x;
    char target;
    char promotion_piece;
} move_t;

typedef struct
{
    move_t move;
    int value;
} move_value_t;


typedef struct
{
    char from_y;
    char from_x;
    char to_y;
    char to_x;
    char promotion_piece;
} user_move_t;

typedef struct
{
    char y;
    char x;
} pair;

#endif
