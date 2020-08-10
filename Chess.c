#include "Chess.h"
#include "ChessInterface.h"
#include "Rate.h"
#include "InOut.h"

const char *NORMAL = "Normal";
const char *EASY = "Easy";

state_t state ={ true, true, true, true, true, false };

char board[8][8] ={
    'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r',  //0-7
    'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p',  //8-15
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  //16-23
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  //24-31
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  //32-39
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  //40-47
    'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P',  //48-55
    'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R' };//56-63

void move_piece(move_t m)
{
    *(*board + m.from) = ' ';

    // Promote pawn if ^
    if (m.piece == '^')
    {
        *(*board + m.to) = 'Q';
    }
    else if (toupper(m.piece) == 'C') // Castling
    {
        if (m.from == 60) // Player castling
        {
            if (m.piece == 'C') // Kingside
            {
                board[7][7] = ' ';
                board[7][6] = 'K';
                board[7][5] = 'R';
            }
            else // Queenside
            {
                board[7][0] = ' ';
                board[7][2] = 'K';
                board[7][3] = 'R';
            }
        }
        else // Computer castling
        {
            if (m.piece == 'C') // Kingside
            {
                board[7][0] = ' ';
                board[7][1] = 'K';
                board[7][2] = 'R';
            }
            else // Queenside
            {
                board[7][7] = ' ';
                board[7][5] = 'K';
                board[7][4] = 'R';
            }
        }
    }
    else
    {
        *(*board + m.to) = m.piece;
    }
}

int main()
{
    // Get settings
    char *settings = get_settings();
    puts(settings);

    // Create a file to log to with a timestamp
    time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    char *time_str = asctime(timeinfo);

    // File names cannot include ';' so use ';' instead
    time_str[13] = ';';
    time_str[16] = ';';
    time_str[24] = '\0'; // This character is \n for some reason
    char log_filename[50];
    snprintf(log_filename, 50, "Logs/%s.txt", time_str);
    // TODO turn this back on
    // fclose(fopen(filename, "w"));

    // Difficulty setting
    if (strcmp(settings, NORMAL) == 0)
    {
        state.public_depth = 4;
    }
    else if (strcmp(settings, EASY) == 0)
    {
        state.public_depth = 0;
    }
    else
    {
        puts("Incorrect Settings! Review the README.");
        exit(EXIT_FAILURE);
    }

    while (true)
    {
        move_t move = get_user_move(board, state);
        log_move(log_filename, move);
        move_piece(move);
    }

    return 0;
}
