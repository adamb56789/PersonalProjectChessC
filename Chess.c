#include "Chess.h"
#include "ChessInterface.h"
#include "Rate.h"
#include "InOut.h"

const char *NORMAL = "Normal";
const char *EASY = "Easy";

state_t state = {true, true, true, true, true, false};

char board[8][8] = 
{
    "rnbqkbnr", //0-7
    "pppppppp", //8-15
    "        ", //16-23
    "        ", //24-31
    "        ", //32-39
    "        ", //40-47
    "PPPPPPPP", //48-55
    "RNBQKBNR", //56-63
};

int main()
{
    char *settings = get_settings();
    puts(settings);
    if (strcmp(settings, NORMAL) == 0)
    {
        state.public_depth = 4;
    }
    else if (strcmp(settings, EASY) == 0)
    {
        state.public_depth = 0;
    } else
    {
        puts("Incorrect Settings! Review the README.");
        exit(EXIT_FAILURE);
    }
    

    return 0;
}
