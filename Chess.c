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
    board[m.from_y][m.from_x] = ' ';

    // Promote pawn if ^
    if (m.piece == '^')
    {
        board[m.to_y][m.to_x] = 'Q';
    }
    else if (toupper(m.piece) == 'C') // Castling
    {
        if (m.from_y == 7 && m.from_x == 4) // Player castling
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
        board[m.to_y][m.to_x] = m.piece;
    }
}

bool safeK(int ox, int oy, int nx, int ny)
{
    return true;
}

int get_pawn_moves(move_t *list, size_t list_length, int y, int x)
{
    // Move one forward
    if (board[y - 1][x] == ' ')
    {
        if (safeK(y, x, y - 1, x))
        {
            list[list_length++] = get_move_from_location(board, y, x, y - 1, x);
        }
        // Move two forward
        if (y == 6)
        {
            // There used to be a try catch here
            if (safeK(y, x, y - 2, x))
            {
                list[list_length++] = get_move_from_location(board, y, x, y - 2, x);
            }
        }
    }

    // Diagonal move (taking)
    for (int i = -1; i <= 1; i += 2) {
        // There used to be a try catch here
        if (islower(board[y - 1][x + i])) {
            if (safeK(y, x, y - 1, x + i)) {
                list[list_length++] = get_move_from_location(board, y, x, y - 1, x + i);
            }
        }
    }
    return list_length;
}

int get_knight_moves(move_t *list, size_t list_length, int x, int y)
{
    return list_length;
}

int get_bishop_moves(move_t *list, size_t list_length, int x, int y)
{
    return list_length;
}

int get_rook_moves(move_t *list, size_t list_length, int x, int y)
{
    return list_length;
}

int get_queen_moves(move_t *list, size_t list_length, int x, int y)
{
    return list_length;
}

int get_king_moves(move_t *list, size_t list_length, int x, int y)
{
    return list_length;
}

move_t* list_moves()
{
    move_t *list = malloc(256 * sizeof(move_t));
    size_t list_length = 0;

    for (int i = 0; i < 8; i++)
    {
        for (size_t j = 0; j < 8; j++)
        {
            switch (board[i][j])
            {
            case 'P': list_length = get_pawn_moves(list, list_length, i, j);
                break;
            case 'N': list_length = get_knight_moves(list, list_length, i, j);
                break;
            case 'B': list_length = get_bishop_moves(list, list_length, i, j);
                break;
            case 'R': list_length = get_rook_moves(list, list_length, i, j);
                break;
            case 'Q': list_length = get_queen_moves(list, list_length, i, j);
                break;
            case 'K': list_length = get_king_moves(list, list_length, i, j);
                break;
            }
        }
    }
    move_t end = {0, 0, 0, 0, 0, 0};
    list[list_length] = end;
    return list;
}

bool is_legal(move_t m)
{
    return true;
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
    free(settings);

    print_board(board, state);

    move_t *moves = list_moves();

    size_t i = 0;
    while (moves[i].piece != 0)
    {
        log_move(moves[i]);
        i++;
    }
    
    

    // while (true)
    // {
    //     move_t move;
    //     do
    //     {
    //         move = get_user_move(board, state);
    //     } while (!is_legal(move));

    //     log_move(move);
    //     move_piece(move);
    // }

    return 0;
}
