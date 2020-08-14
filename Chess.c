#include "Chess.h"
#include "ChessInterface.h"
#include "Rate.h"
#include "InOut.h"

#define in_bounds(x) 0 <= x && x <= 7

#define swap_case(x) x = isupper(x) ? tolower(x) : toupper(x)

/* TODO
    bug: en passant reveal-checking oneself is not prevented
    stalemate rules except no valid moves
*/

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
const move_t KINGSIDE_BLACK ={ 'C', 7, 3, 7, 1, ' ' };
const move_t QUEENSIDE_BLACK ={ 'c', 7, 3, 7, 5, ' ' };

char STARTING_POSITION[] =
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
// char STARTING_POSITION[] =
//     "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1";
char KIWIPETE[] = 
    "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ";

char board[8][8] = {};

// ep_x (en passant target) is stored from the opposing side's perspective after making a move
state_t game ={ white, false, false, false, false, false, false, -1 };


const int MAX_LIST_LENGTH = 255;
int moves_len;

void rotate_board() // swaps piece colour as well as move pieces
{
    game.turn = game.turn == white ? black : white;
    for (int y = 0; y < 8; y++)
    {
        for (int x = 0; x < 4; x++)
        {
            int ry = 7 - y;
            int rx = 7 - x;
            swap_case(board[y][x]);
            swap_case(board[ry][rx]);
            char temp = board[y][x];
            board[y][x] = board[ry][rx];
            board[ry][rx] = temp;
        }
    }
}

void initialise_board(char *fen)
{
    memset(board, ' ', 64);
    int i = 0;
    int y = 0;
    int x = 0;
    bool play_as_black = false;
    while (fen[i] != ' ')
    {
        if (fen[i] == '/')
        {
            y++;
            x = 0;
        }
        else if (isdigit(fen[i]))
        {
            x += fen[i] - 48;;
        }
        else
        {
            board[y][x++] = fen[i];
        }
        i++;
    }
    i++;
    play_as_black = fen[i] == 'b';
    i += 2;
    while (fen[i] != ' ')
    {
        switch (fen[i])
        {
        case 'K':
            game.K_stationary = true;
            game.kingside_R_stationary = true;
            break;
        case 'Q':
            game.K_stationary = true;
            game.queenside_R_stationary = true;
            break;
        case 'k':
            game.k_stationary = true;
            game.kingside_r_stationary = true;
            break;
        case 'q':
            game.k_stationary = true;
            game.queenside_r_stationary = true;
            break;
        default:
            break;
        }
        i++;
    }
    if (play_as_black)
    {
        rotate_board();
    }
}

void make_move(move_t m)
{
    board[m.from_y][m.from_x] = ' ';
    board[m.to_y][m.to_x] = m.piece;
    game.ep_x = -1; // default to no en passant target

    if (m.piece == '^') // Promotion
    {
        // default to queen
        board[m.to_y][m.to_x] = m.promotion_piece == ' ' ? 'Q' : m.promotion_piece;
    }
    else if (m.piece == 'P' && m.from_y == 6 && m.to_y == 4) // pawn double move
    {
        game.ep_x = 7 - m.from_x; // record en passant target
    }
    else if (m.piece == 'E') // en passant
    {
        board[m.to_y][m.to_x] = 'P';
        board[m.from_y][m.to_x] = ' ';
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
                game.kingside_R_stationary = false;
            }
            else // Black
            {
                board[7][0] = ' ';
                board[7][1] = 'K';
                board[7][2] = 'R';
                game.kingside_r_stationary = false;
            }
        }
        else if (m.piece == 'c') // Queenside castle
        {
            if (m.from_x == 4) // White
            {
                board[7][0] = ' ';
                board[7][2] = 'K';
                board[7][3] = 'R';
                game.queenside_R_stationary = false;
            }
            else // Black
            {
                board[7][7] = ' ';
                board[7][5] = 'K';
                board[7][4] = 'R';
                game.queenside_r_stationary = false;
            }
        }

        if (m.from_x == 0) // left rook movement
        {
            if (game.turn == white)
            {
                game.queenside_R_stationary = false;
            }
            else
            {
                game.kingside_r_stationary = false;
            }
        }
        else if (m.from_x == 7) // right rook movement
        {
            if (game.turn == white)
            {
                game.kingside_R_stationary = false;
            }
            else
            {
                game.queenside_r_stationary = false;
            }
        }
        else if (game.turn == white && m.from_x == 4) // white king movement
        {
            game.K_stationary = false;
        }
        else if (game.turn == black && m.from_x == 3) // black king movement
        {
            game.k_stationary = false;
        }
    }
}

void undo_move(move_t m, int old_ep_x)
{
    board[m.from_y][m.from_x] = m.piece;
    board[m.to_y][m.to_x] = m.target;
    game.ep_x = old_ep_x;

    if (m.piece == '^') // Promotion
    {
        board[m.from_y][m.from_x] = 'P';
    }
    else if (m.piece == 'E') // en passant
    {
        board[m.from_y][m.from_x] = 'P';
        board[m.from_y][m.to_x] = 'p';
    }
    else if (m.from_y == 7) // a lot of special things happen on the back rank
    {
        if (m.piece == 'C') // Kingside castle
        {
            if (m.from_x == 4) // White
            {
                board[7][7] = 'R';
                board[7][6] = ' ';
                board[7][5] = ' ';
                board[7][4] = 'K';
                game.kingside_R_stationary = true;
            }
            else // Black
            {
                board[7][0] = 'R';
                board[7][1] = ' ';
                board[7][2] = ' ';
                board[7][3] = 'K';
                game.kingside_r_stationary = true;
            }
        }
        else if (m.piece == 'c') // Queenside castle
        {
            if (m.from_x == 4) // White
            {
                board[7][0] = 'R';
                board[7][2] = ' ';
                board[7][3] = ' ';
                board[7][4] = 'K';
                game.queenside_R_stationary = true;
            }
            else // Black
            {
                board[7][7] = 'R';
                board[7][5] = ' ';
                board[7][4] = ' ';
                board[7][3] = 'K';
                game.queenside_r_stationary = true;
            }
        }

        if (m.from_x == 0) // left rook movement
        {
            if (game.turn == white)
            {
                game.queenside_R_stationary = true;
            }
            else
            {
                game.kingside_r_stationary = true;
            }
        }
        else if (m.from_x == 7) // right rook movement
        {
            if (game.turn == white)
            {
                game.kingside_R_stationary = true;
            }
            else
            {
                game.queenside_r_stationary = true;
            }
        }
        else if (game.turn == white && m.from_x == 4) // white king movement
        {
            game.K_stationary = true;
        }
        else if (game.turn == black && m.from_x == 3) // black king movement
        {
            game.k_stationary = true;
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

bool is_attacked(pair square)
{
    int ky = square.y;
    int kx = square.x;

    // Pawn on left
    if (1 < ky // Simple y bounds check
        && in_bounds(kx - 1)
        && board[ky - 1][kx - 1] == 'p')
    {
        return true;
    }
    // Pawn on right
    if (1 < ky // Simple y bounds check
        && in_bounds(kx + 1)
        && board[ky - 1][kx + 1] == 'p')
    {
        return true;
    }

    // Knights
    for (int i = 0; i < 8; i++)
    {
        int ny = ky + KNIGHT_JUMPS[i].y;
        int nx = kx + KNIGHT_JUMPS[i].x;
        if (in_bounds(ny) && in_bounds(nx) && board[ny][nx] == 'n')
        {
            return true;
        }
    }

    // Queens, rooks, and bishops
    for (int i = 0; i < 8; i++)
    {
        int move_length = 1;
        while (true)
        {
            int ny = ky + QUEEN_DIRECTIONS[i].y * move_length;
            int nx = kx + QUEEN_DIRECTIONS[i].x * move_length;
            move_length++;

            bool on_board = in_bounds(ny) && in_bounds(nx);
            bool straight = (QUEEN_DIRECTIONS[i].y == 0) || (QUEEN_DIRECTIONS[i].x == 0);
            char piece = board[ny][nx];

            if (on_board && (
                piece == 'q'
                || piece == 'r' && straight
                || piece == 'b' && !straight))
            {
                return true;
            }
            
            // If the search can go no further on that line
            if (!(on_board && piece == ' '))
            {
                break;
            }
        }
    }

    // King
    for (int i = 0; i < 8; i++)
    {
        int ny = ky + KING_JUMPS[i].y;
        int nx = kx + KING_JUMPS[i].x;
        if (in_bounds(ny) && in_bounds(nx) && board[ny][nx] == 'k')
        {
            return true;
        }
    }

    return false;
}

bool is_attacked_yx(int y, int x) // this makes things easier in generate_castling
{
    pair p ={y, x};
    return is_attacked(p);
}

bool move_is_safe(move_t m)
{
    int old_ep_x = game.ep_x;
    make_move(m);
    bool safe = !is_attacked(get_my_king_location());
    undo_move(m, old_ep_x);
    return safe;
}

void generate_pawn_moves(move_t *moves, int y, int x)
{
    if (board[y - 1][x] == ' ')
    {
        // Move one forward
        move_t m ={ 'P', y, x, y - 1, x, board[y - 1][x] };
        if (move_is_safe(m))
        {
            if (y == 1) // Promotion
            {
                move_t mq ={ '^', y, x, y - 1, x, board[y - 1][x], 'Q' };
                move_t mr ={ '^', y, x, y - 1, x, board[y - 1][x], 'R' };
                move_t mb ={ '^', y, x, y - 1, x, board[y - 1][x], 'B' };
                move_t mn ={ '^', y, x, y - 1, x, board[y - 1][x], 'N' };
                moves[moves_len++] = mq;
                moves[moves_len++] = mr;
                moves[moves_len++] = mb;
                moves[moves_len++] = mn;
            }
            else
            {
                moves[moves_len++] = m;
            }
        }
        // Move two forward
        // There used to be a try catch here on safeK, I don't think it was necessary
        move_t m_2 ={ 'P', y, x, y - 2, x, board[y - 2][x] };
        if (y == 6 && board[y - 2][x] == ' '
            && move_is_safe(m_2))
        {
            moves[moves_len++] = m_2;
        }
    }

    // Diagonal moves
    for (int i = -1; i <= 1; i += 2)
    {
        if (in_bounds(x + i))
        {
            move_t m ={ 'P', y, x, y - 1, x + i, board[y - 1][x + i] };
            if (islower(board[y - 1][x + i]) && move_is_safe(m))
            {
                if (y == 1) // Promotion
                {
                    move_t mq ={ '^', y, x, y - 1, x + i, board[y - 1][x + i], 'Q' };
                    move_t mr ={ '^', y, x, y - 1, x + i, board[y - 1][x + i], 'R' };
                    move_t mb ={ '^', y, x, y - 1, x + i, board[y - 1][x + i], 'B' };
                    move_t mn ={ '^', y, x, y - 1, x + i, board[y - 1][x + i], 'N' };
                    moves[moves_len++] = mq;
                    moves[moves_len++] = mr;
                    moves[moves_len++] = mb;
                    moves[moves_len++] = mn;
                }
                else
                {
                    moves[moves_len++] = m;
                }
            }
            else if (y == 3 && x + i == game.ep_x) // En passant
            {
                m.piece = 'E';
                moves[moves_len++] = m;
            }
        }
    }
}

void generate_jumping_moves(move_t *moves, int y, int x, pair *jumps, int jumps_length)
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
                moves[moves_len++] = m;
            }
        }
    }
}

void generate_linear_moves(move_t *moves, int y, int x, pair *directions, int directions_length)
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
                    moves[moves_len++] = m;
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

void generate_castling_moves(move_t *moves, int y, int x) // causes king to appear at spawnpoint
{
    pair king_square ={y, x};
    if (game.turn == white && game.K_stationary)
    {
        if (game.kingside_R_stationary
            && board[7][6] == ' '
            && board[7][5] == ' '
            && !is_attacked(king_square)
            && !is_attacked_yx(7, 5) // no castling through attacked square
            && move_is_safe(KINGSIDE_WHITE))
        {
            moves[moves_len++] = KINGSIDE_WHITE;
        }
        if (game.queenside_R_stationary
            && board[7][1] == ' '
            && board[7][2] == ' '
            && board[7][3] == ' '
            && !is_attacked(king_square)
            && !is_attacked_yx(7, 4)
            && move_is_safe(QUEENSIDE_WHITE))
        {
            moves[moves_len++] = QUEENSIDE_WHITE;
        }
    }
    else if (game.turn == black && game.k_stationary)
    {
        if (game.queenside_r_stationary
            && board[7][6] == ' '
            && board[7][5] == ' '
            && board[7][4] == ' '
            && !is_attacked(king_square)
            && !is_attacked_yx(7, 4)
            && move_is_safe(QUEENSIDE_BLACK))
        {
            moves[moves_len++] = QUEENSIDE_BLACK;
        }
        if (game.kingside_r_stationary
            && board[7][1] == ' '
            && board[7][2] == ' '
            && !is_attacked(king_square)
            && !is_attacked_yx(7, 2)
            && move_is_safe(KINGSIDE_BLACK))
        {
            moves[moves_len++] = KINGSIDE_BLACK;
        }
    }
}

// moves_len keeps track of the current location in the moves list
// is global variable to reduce function arguments
int generate_moves(move_t *moves)
{
    moves_len = 0;
    for (int y = 0; y < 8; y++)
    {
        for (int x = 0; x < 8; x++)
        {
            switch (board[y][x])
            {
            case 'P': generate_pawn_moves(moves, y, x);
                break;
            case 'N': generate_jumping_moves(moves, y, x, KNIGHT_JUMPS, 8);
                break;
            case 'B': generate_linear_moves(moves, y, x, BISHOP_DIRECTIONS, 4);
                break;
            case 'R': generate_linear_moves(moves, y, x, ROOK_DIRECTIONS, 4);
                break;
            case 'Q': generate_linear_moves(moves, y, x, QUEEN_DIRECTIONS, 8);
                break;
            case 'K':
                generate_jumping_moves(moves, y, x, KING_JUMPS, 8);
                generate_castling_moves(moves, y, x);
                break;
            }
        }
    }
    return moves_len;
}

bool is_legal_move(move_t m)
{
    move_t *moves = malloc(MAX_LIST_LENGTH * sizeof(move_t));
    int number_of_moves = generate_moves(moves);
    for (int i = 0; i < number_of_moves; i++)
    {
        // this doesn't need to check for promotion piece
        // as that can never affect legality
        if (m.from_y == moves[i].from_y
            && m.from_x == moves[i].from_x
            && m.to_y == moves[i].to_y
            && m.to_x == moves[i].to_x)
        {
            return true;
        }
    }
    puts("That is not a legal move");
    return false;
}

move_t get_move_pieces(user_move_t user_m)
{
    move_t m ={ board[user_m.from_y][user_m.from_x],
        user_m.from_y,
        user_m.from_x,
        user_m.to_y,
        user_m.to_x,
        board[user_m.to_y][user_m.to_x],
        m.promotion_piece = user_m.promotion_piece };

    if (m.piece == 'P' && m.to_y == 0) // Promotion
    {
        m.piece = '^';
    }
    else if (m.piece == 'K' && m.from_y == 7) // Castling attempt
    {
        if (m.from_x == 4 && m.to_x == 6 || m.from_x == 3 && m.to_x == 1)
        {
            m.piece = 'C'; // Kingside
        }
        else if (m.from_x == 4 && m.to_x == 2 || m.from_x == 3 && m.to_x == 5)
        {
            m.piece = 'c'; // Queenside
        }
    }
    else if (m.piece == 'P' && m.from_y == 3 && abs(m.to_x - m.from_x) == 1 && m.target == ' ')
    {
        m.piece = 'E'; // En passant attempt
    }
    
    return m;
}

int nodes[16];
int captures[16];
int eps[16];
int castles[16];

// Doesn't do any rating or searching yet, just makes a tree for debugging
move_t alpha_beta(int depth, int beta, int alpha, move_t move)
{
    nodes[depth]++;
    if (move.target != ' ')
    {
        captures[depth]++;
    }
    if (move.piece == 'E')
    {
        eps[depth]++;
        captures[depth]++;
    }
    if (toupper(move.piece) == 'C')
    {
        castles[depth]++;
    }
    
    if (depth == 0)
    {
        return move;
    }

    move_t *moves = malloc(MAX_LIST_LENGTH * sizeof(move_t));
    int number_of_moves = generate_moves(moves);

    for (int i = 0; i < number_of_moves; i++)
    {
        int old_ep_x = game.ep_x;
        make_move(moves[i]);
        log_move(moves[i]);
        print_board(board, game);
        rotate_board();
        move_t return_move = alpha_beta(depth - 1, beta, alpha, moves[i]);
        rotate_board();
        undo_move(moves[i], old_ep_x);
    }
    return move;
}

int main()
{
    initialise_board("8/5bk1/8/2Pp4/8/1K6/8/8 w - d6 0 1");
    game.ep_x = 3;
    print_board(board, game);

    int test_depth = 1;
    alpha_beta(test_depth, 0, 0, KINGSIDE_WHITE);
    for (int i = test_depth - 1; i >= 0; i--)
    {
        printf("%d  %d  %d  %d\n", nodes[i], captures[i], eps[i], castles[i]);
    }
    
    return 0;

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
    int max_depth;
    if (strcmp(settings, NORMAL) == 0)
    {
        max_depth = 4;
    }
    else if (strcmp(settings, EASY) == 0)
    {
        max_depth = 0;
    }
    else
    {
        puts("Incorrect Settings! Review the README.");
        exit(EXIT_FAILURE);
    }
    free(settings);

    while (true)
    {
        print_board(board, game);
        move_t move;
        do
        {
            move = get_move_pieces(get_user_move(game.turn));
        } while (!is_legal_move(move));

        log_move(move);
        make_move(move);
        rotate_board();

        move_t *moves = malloc(MAX_LIST_LENGTH * sizeof(move_t));
        int number_of_moves = generate_moves(moves);
        if (number_of_moves == 0)
        {
            if (is_attacked(get_my_king_location()))
            {
                rotate_board();
                print_board(board, game);
                puts("Checkmate!");
                return 0;
            }
            else
            {
                rotate_board();
                print_board(board, game);
                puts("Stalemate");
                return 0;
            }
        }
    }
}
