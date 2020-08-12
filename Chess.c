#include "Chess.h"
#include "ChessInterface.h"
#include "Rate.h"
#include "InOut.h"

#define in_bounds(x) 0 <= x && x <= 7

// TODO
// en passant
// promotions other than queen
// castling out of check
// stalemate rules

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

const move_t KINGSIDE_WHITE ={ 'C', 7, 4, 7, 6, ' ' };
const move_t QUEENSIDE_WHITE ={ 'c', 7, 4, 7, 2, ' ' };
const move_t KINGSIDE_BLACK ={ 'C', 7, 3, 7, 2, ' ' };
const move_t QUEENSIDE_BLACK ={ 'c', 7, 3, 7, 6, ' ' };

// left_r and right_r are from black's perspective - right_r is on [0][0] below
state_t game ={ white, true, true, true, true, true, true };

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
    board[m.to_y][m.to_x] = m.piece;

    if (m.piece == '^') // Promotion
    {
        board[m.to_y][m.to_x] = 'Q';
    }
    else if (m.from_y == 7) // a lot of special things happen on the back rank
    {
        if (m.piece == 'C') // Kingside castle
        {
            if (m.from_x == 4) // White
            {
                board[7][7] = ' '; // move the rook as well
                board[7][6] = 'K';
                board[7][5] = 'R';
                game.right_R_stationary = false;
            }
            else // Black
            {
                board[7][0] = ' ';
                board[7][1] = 'K';
                board[7][2] = 'R';
                game.left_r_stationary = false;
            }
        }
        else if (m.piece == 'c') // Queenside castle
        {
            if (m.from_x == 4) // White
            {
                board[7][0] = ' ';
                board[7][2] = 'K';
                board[7][3] = 'R';
                game.left_R_stationary = false;
            }
            else // Black
            {
                board[7][7] = ' ';
                board[7][5] = 'K';
                board[7][4] = 'R';
                game.right_r_stationary = false;
            }
        }

        if (m.from_x == 0) // left rook movement
        {
            if (game.turn == white)
            {
                game.left_R_stationary = false;
            }
            else
            {
                game.left_r_stationary = false;
            }
        }
        else if (m.from_x == 7) // right rook movement
        {
            if (game.turn == white)
            {
                game.right_R_stationary = false;
            }
            else
            {
                game.right_r_stationary = false;
            }
        }
        else if (game.turn == white && m.from_x == 4) // white king movement
        {
            game.K_stationary = false;
        }
        else if (game.turn == black && m.from_y == 3) // black king movement
        {
            game.k_stationary = false;
        }
    }
}

pair get_my_king_location()
{
    for (int y = 0; y < 8; y++)
    {
        for (int x = 0; x < 8; x++)
        {
            if (board[y][x] == 'K')
            {
                pair location ={y, x};
                return location;
            }
        }
    }
}

bool is_in_check()
{
    return false;
}

bool move_is_safe(move_t m)
{
    return true;
}

void generate_pawn_moves(int y, int x)
{
    // Move one forward
    if (board[y - 1][x] == ' ')
    {
        move_t m ={ board[y][x], y, x, y - 1, x, board[y - 1][x] };
        if (move_is_safe(m))
        {
            list[list_length++] = m;
        }
        // Move two forward
        // There used to be a try catch here on safeK, I don't think it was necessary
        move_t m_2 ={ board[y][x], y, x, y - 2, x, board[y - 2][x] };
        if (y == 6 && board[y - 2][x] == ' '
            && move_is_safe(m_2))
        {
            list[list_length++] = m_2;
        }
    }

    // Diagonal capture
    for (int i = -1; i <= 1; i += 2)
    {
        if (in_bounds(x + i))
        {
            move_t m ={ board[y][x], y, x, y - 1, x + i, board[y - 1][x + i] };
            if (islower(board[y - 1][x + i]) && move_is_safe(m))
            {
                list[list_length++] = m;
            }
        }
    }
}

void generate_jumping_moves(int y, int x, pair *jumps, int jumps_length)
{
    for (int i = 0; i < jumps_length; i++)
    {
        // new y and new x
        int ny = y + jumps[i].y;
        int nx = x + jumps[i].x;

        if (in_bounds(ny) && in_bounds(nx))
        {
            move_t m ={ board[y][x], y, x, ny, nx, board[ny][nx] };
            if (!isupper(board[ny][nx]) && move_is_safe(m))
            {
                list[list_length++] = m;
            }
        }
    }
}

void generate_linear_moves(int y, int x, pair *directions, int directions_length)
{
    for (int i = 0; i < directions_length; i++)
    {
        int move_length = 1;
        while (true)
        {
            // new y and new x
            int ny = y + directions[i].y * move_length;
            int nx = x + directions[i].x * move_length;
            move_length++;

            bool on_board = in_bounds(ny) && in_bounds(nx);

            // Move if possible
            if (on_board)
            {
                move_t m ={ board[y][x], y, x, ny, nx, board[ny][nx] };
                if (!isupper(board[ny][nx]) && move_is_safe(m))
                {
                    list[list_length++] = m;
                }
            }
            
            // If out of bounds or if the move was a capture then
            // stop getting moves from that line
            if (!(on_board && board[ny][nx] == ' '))
            {
                break;
            }
        }
    }
}

void generate_castling_moves(int y, int x)
{
    if (game.turn == white
        && game.K_stationary)
    {
        if (game.right_R_stationary
            && board[7][6] == ' '
            && board[7][5] == ' '
            && !is_in_check()
            && move_is_safe(KINGSIDE_WHITE))
        {
            list[list_length++] = KINGSIDE_WHITE;
        }
        if (game.left_R_stationary
            && board[7][1] == ' '
            && board[7][2] == ' '
            && board[7][3] == ' '
            && !is_in_check()
            && move_is_safe(QUEENSIDE_WHITE))
        {
            list[list_length++] = QUEENSIDE_WHITE;
        }
    }
    else if (game.k_stationary)
    {
        if (game.right_r_stationary
            && board[7][6] == ' '
            && board[7][5] == ' '
            && board[7][4] == ' '
            && !is_in_check()
            && move_is_safe(QUEENSIDE_BLACK))
        {
            list[list_length++] = QUEENSIDE_BLACK;
        }
        if (game.left_r_stationary
            && board[7][1] == ' '
            && board[7][2] == ' '
            && !is_in_check()
            && move_is_safe(KINGSIDE_BLACK))
        {
            list[list_length++] = KINGSIDE_BLACK;
        }
    }
}

void generate_moves()
{
    list_length = 0;
    for (int y = 0; y < 8; y++)
    {
        for (int x = 0; x < 8; x++)
        {
            switch (board[y][x])
            {
            case 'P': generate_pawn_moves(y, x);
                break;
            case 'N': generate_jumping_moves(y, x, KNIGHT_JUMPS, 8);
                break;
            case 'B': generate_linear_moves(y, x, BISHOP_DIRECTIONS, 4);
                break;
            case 'R': generate_linear_moves(y, x, ROOK_DIRECTIONS, 4);
                break;
            case 'Q': generate_linear_moves(y, x, QUEEN_DIRECTIONS, 8);
                break;
            case 'K':
                generate_jumping_moves(y, x, KING_JUMPS, 8);
                generate_castling_moves(y, x);
                break;
            }
        }
    }
}

bool is_legal_move(move_t m)
{
    generate_moves();
    for (int i = 0; i < list_length; i++)
    {
        if (m.from_y == list[i].from_y
            && m.from_x == list[i].from_x
            && m.to_y == list[i].to_y
            && m.to_x == list[i].to_x)
        {
            return true;
        }
    }
    puts("That is not a legal move");
    return false;
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
        game.public_depth = 4;
    }
    else if (strcmp(settings, EASY) == 0)
    {
        game.public_depth = 0;
    }
    else
    {
        puts("Incorrect Settings! Review the README.");
        exit(EXIT_FAILURE);
    }
    free(settings);

    print_board(board, game);

    list = malloc(MAX_LIST_LENGTH * sizeof(move_t));
    // generate_moves();
    // for (int i = 0; i < list_length; i++)
    // {
    //     log_move(list[i]);
    // }

    while (true)
    {
        print_board(board, game);
        move_t move;
        do
        {
            move = get_user_move(board, game);
        } while (!is_legal_move(move));

        log_move(move);
        move_piece(move);
    }

    return 0;
}
