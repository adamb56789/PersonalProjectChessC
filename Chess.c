#include "Chess.h"
#include "ChessInterface.h"
#include "Rate.h"
#include "InOut.h"

#define in_bounds(x) 0 <= x && x <= 7


const char *NORMAL = "Normal";
const char *EASY = "Easy";

// (y, x) specifies how a piece can jump move
pair KNIGHT_JUMPS[8] ={ -1, -2, -2, -1, -1, 2, -2, 1, 1, -2, 2, -1, 1, 2, 2, 1 };
pair KING_JUMPS[8] ={ -1, -1, -1, 0, -1, 1, 0, -1, 0, 1, 1, -1, 1, 0, 1, 1 };

// (y, x) specifies how a piece can move in a line
// -1 indicates that the piece can move in that coordinate in the negative direction
pair BISHOP_DIRECTIONS[4] ={ -1, -1, -1, 1, 1, -1, 1, 1 };
pair ROOK_DIRECTIONS[4] ={ -1, 0, 0, -1, 0, 1, 1, 0 };
pair QUEEN_DIRECTIONS[8] ={ -1, -1, -1, 0, -1, 1, 0, -1, 0, 1, 1, -1, 1, 0, 1, 1 };

state_t state ={ true, true, true, true, true, false, 0 };

// char board[8][8] ={
//     'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r',  //0-7
//     'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p',  //8-15
//     ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  //16-23
//     ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  //24-31
//     ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  //32-39
//     ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  //40-47
//     'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P',  //48-55
//     'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R' };//56-63

char board[8][8] ={
    'r', ' ', ' ', 'q', ' ', 'r', 'k', ' ',  //0-7
    ' ', 'b', ' ', ' ', 'p', 'p', 'b', ' ',  //8-15
    ' ', 'p', ' ', ' ', ' ', ' ', 'p', 'p',  //16-23
    'p', ' ', 'n', 'n', 'N', ' ', ' ', ' ',  //24-31
    'P', 'P', ' ', ' ', ' ', ' ', ' ', 'N',  //32-39
    ' ', ' ', 'P', ' ', ' ', ' ', ' ', ' ',  //40-47
    ' ', ' ', 'B', 'B', ' ', 'P', 'P', 'P',  //48-55
    'R', ' ', ' ', 'Q', 'K', ' ', ' ', 'R' };//56-63

move_t *list;
const int MAX_LIST_LENGTH = 255;
int list_length;

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

int generate_pawn_moves(int y, int x)
{
    // Move one forward
    if (board[y - 1][x] == ' ')
    {
        if (safeK(y, x, y - 1, x))
        {
            list[list_length++] = to_move(board, y, x, y - 1, x);
        }
        // Move two forward
        // There used to be a try catch here on safeK, I don't think it was necessary
        if (y == 6
            && safeK(y, x, y - 2, x))
        {
            list[list_length++] = to_move(board, y, x, y - 2, x);
        }
    }

    // Diagonal capture
    for (int i = -1; i <= 1; i += 2)
    {
        if (in_bounds(x + i)
            && islower(board[y - 1][x + i])
            && safeK(y, x, y - 1, x + i))
        {
            list[list_length++] = to_move(board, y, x, y - 1, x + i);
        }
    }
    return list_length;
}

int generate_jumping_moves(int y, int x, pair *jumps, int jumps_length)
{
    for (int i = 0; i < jumps_length; i++)
    {
        // new y and new x
        int ny = y + jumps[i].a;
        int nx = x + jumps[i].b;

        if (in_bounds(ny)
            && in_bounds(nx)
            && !isupper(board[ny][nx])
            && safeK(y, x, ny, nx))
        {
            list[list_length++] = to_move(board, y, x, ny, nx);
        }
    }
    return list_length;
}

int generate_linear_moves(int y, int x, pair *directions, int directions_length)
{
    for (int i = 0; i < directions_length; i++)
    {
        int move_length = 1;
        while (true)
        {
            // new y and new x
            int ny = y + directions[i].a * move_length;
            int nx = x + directions[i].b * move_length;
            move_length++;

            bool on_board = in_bounds(ny) && in_bounds(nx);

            // Move if possible
            if (on_board
                && !isupper(board[ny][nx])
                && safeK(y, x, ny, nx))
            {
                list[list_length++] = to_move(board, y, x, ny, nx);
            }
            // If out of bounds or if the move was a capture then
            // stop getting moves from that line
            if (!(on_board && board[ny][nx] == ' '))
            {
                break;
            }
        }
    }
    return list_length;
}

int generate_castling_moves(int y, int x)
{
    return list_length;
}

void generate_moves()
{
    for (int y = 0; y < 8; y++)
    {
        for (int x = 0; x < 8; x++)
        {
            switch (board[y][x])
            {
            case 'P': list_length = generate_pawn_moves(y, x);
                break;
            case 'N': list_length = generate_jumping_moves(y, x, KNIGHT_JUMPS, 8);
                break;
            case 'B': list_length = generate_linear_moves(y, x, BISHOP_DIRECTIONS, 4);
                break;
            case 'R': list_length = generate_linear_moves(y, x, ROOK_DIRECTIONS, 4);
                break;
            case 'Q': list_length = generate_linear_moves(y, x, QUEEN_DIRECTIONS, 8);
                break;
            case 'K': list_length = generate_jumping_moves(y, x, KING_JUMPS, 8);
                break;
            }
        }
    }
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

    list = malloc(MAX_LIST_LENGTH * sizeof(move_t));
    list_length = 0;
    generate_moves();

    for (int i = 0; i < list_length; i++)
    {
        log_move(list[i]);
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
