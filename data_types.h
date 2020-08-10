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
