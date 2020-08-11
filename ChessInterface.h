#include "data_types.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

move_t to_move(char board[8][8], int from_y, int from_x, int to_y, int to_x);

void print_board(char board[8][8], state_t state);

move_t get_user_move(char board[8][8], state_t state);
