#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "Rate.h"

int first = 1;

// https://www.chessprogramming.org/Simplified_Evaluation_Function
int rate(char board[8][8], int number_possible_moves, int depth)
{
    if (first)
    {
        srand(time(NULL));
        first = 0;
    }
    int r = (rand() % 1000) - 500;
    return r;
}

int rate_material(char board[8][8])
{

}

int rate_location(char board[8][8], int material_rating)
{

}

int rate_moves(int number_possible_moves, int depth)
{

}
