#include <stdio.h>
#include <stdbool.h>
#include <string.h>

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
