#include "Chess.h"
#include "ChessInterface.h"
#include "Rate.h"
#include "InOut.h"

#define in_bounds(x) 0 <= x && x <= 7

#define swap_case(x) x = isupper(x) ? tolower(x) : toupper(x)

/* TODO
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

const move_t BLANK_MOVE ={0, 0, 0, 0, 0, 0};
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
state_t state ={ white, false, false, false, false, -1 };

int max_depth;

const int MAX_LIST_LENGTH = 255;
int moves_len;

move_t rotate_move(move_t m)
{
    m.from_x = 7 - m.from_x;
    m.from_y = 7 - m.from_y;
    m.to_x = 7 - m.to_x;
    m.to_y = 7 - m.to_y;
    return m;
}

void rotate_board() // swaps piece colour as well as move pieces
{
    state.turn = state.turn == white ? black : white;
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
    state_t new_state = { white, false, false, false, false, -1 };
    state = new_state;
    
    int i = 0;
    int y = 0;
    int x = 0;
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
    bool play_as_black = fen[i] == 'b';
    i += 2;
    while (fen[i] != ' ')
    {
        switch (fen[i])
        {
        case 'K':
            state.can_castle_K = true;
            break;
        case 'Q':
            state.can_castle_Q = true;
            break;
        case 'k':
            state.can_castle_k = true;
            break;
        case 'q':
            state.can_castle_q = true;
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
    state.ep_x = -1; // default to no en passant target

    if (m.piece == '^') // Promotion
    {
        // default to queen
        board[m.to_y][m.to_x] = m.promotion_piece == ' ' ? 'Q' : m.promotion_piece;
    }
    else if (m.piece == 'P' && m.from_y == 6 && m.to_y == 4) // pawn double move
    {
        state.ep_x = 7 - m.from_x; // record en passant target
    }
    else if (m.piece == 'E') // en passant
    {
        board[m.to_y][m.to_x] = 'P';
        board[m.from_y][m.to_x] = ' ';
    }
    else if (m.piece == 'C') // Kingside castle
    {
        if (state.turn == white)
        {
            board[7][7] = ' '; // move the rook as well
            board[7][6] = 'K';
            board[7][5] = 'R';
            state.can_castle_K = false;
            state.can_castle_Q = false;
        }
        else
        {
            board[7][0] = ' ';
            board[7][1] = 'K';
            board[7][2] = 'R';
            state.can_castle_k = false;
            state.can_castle_q = false;
        }
    }
    else if (m.piece == 'c') // Queenside castle
    {
        if (state.turn == white) // White
        {
            board[7][0] = ' ';
            board[7][2] = 'K';
            board[7][3] = 'R';
            state.can_castle_K = false;
            state.can_castle_Q = false;
        }
        else // Black
        {
            board[7][7] = ' ';
            board[7][5] = 'K';
            board[7][4] = 'R';
            state.can_castle_k = false;
            state.can_castle_q = false;
        }
    }
    else if (m.piece == 'R') // rook movement
    {
        if (state.turn == white)
        {
            if (m.from_x == 7)
            {
                state.can_castle_K = false;
            }
            else if (m.from_x == 0)
            {
                state.can_castle_Q = false;
            }
        }
        else
        {
            if (m.from_x == 7)
            {
                state.can_castle_q = false;
            }
            else if (m.from_x == 0)
            {
                state.can_castle_k = false;
            }
        }
    }
    else if (m.piece == 'K') // King moves
    {
        if (state.turn == white)
        {
            state.can_castle_K = false;
            state.can_castle_Q = false;
        }
        else
        {
            state.can_castle_k = false;
            state.can_castle_q = false;
        }
    }

    if (m.target == 'r' && m.to_y == 0) // capture enemy rook
    {
        if (state.turn == white)
        {
            if (m.to_x == 7)
            {
                state.can_castle_k = false;
            }
            else if (m.to_x == 0)
            {
                state.can_castle_q = false;
            }
        }
        else
        {
            if (m.to_x == 7)
            {
                state.can_castle_Q = false;
            }
            else if (m.to_x == 0)
            {
                state.can_castle_K = false;
            }
        }
    }
}

void undo_move(move_t m, state_t old_state)
{
    board[m.from_y][m.from_x] = m.piece;
    board[m.to_y][m.to_x] = m.target;
    state = old_state;

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
            }
            else // Black
            {
                board[7][0] = 'R';
                board[7][1] = ' ';
                board[7][2] = ' ';
                board[7][3] = 'K';
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
            }
            else // Black
            {
                board[7][7] = 'R';
                board[7][5] = ' ';
                board[7][4] = ' ';
                board[7][3] = 'K';
            }
        }
    }
}

pair get_my_king_location()
{
    int n = 0;
    pair location;
    for (int y = 0; y < 8; y++)
    {
        for (int x = 0; x < 8; x++)
        {
            if (board[y][x] == 'K')
            {
                pair temp ={y, x};
                location = temp;
                n++;
            }
        }
    }
    // TODO remove debug
    if (n != 1)
    {
        print_board(board, state);
        printf("ERROR: there are %d kings!", n);
        exit(EXIT_FAILURE);
    }
    return location;
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
    state_t old_state = state;
    make_move(m);
    bool safe = !is_attacked(get_my_king_location());
    undo_move(m, old_state);
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
            else if (y == 3 && x + i == state.ep_x) // En passant
            {
                m.piece = 'E';
                if (move_is_safe(m))
                {
                    moves[moves_len++] = m;
                }
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
    if (state.turn == white)
    {
        if (state.can_castle_K
            && board[7][6] == ' '
            && board[7][5] == ' '
            && !is_attacked_yx(y, x)
            && !is_attacked_yx(7, 5) // no castling through attacked square
            && move_is_safe(KINGSIDE_WHITE))
        {
            moves[moves_len++] = KINGSIDE_WHITE;
        }
        if (state.can_castle_Q
            && board[7][1] == ' '
            && board[7][2] == ' '
            && board[7][3] == ' '
            && !is_attacked_yx(y, x)
            && !is_attacked_yx(7, 3)
            && move_is_safe(QUEENSIDE_WHITE))
        {
            moves[moves_len++] = QUEENSIDE_WHITE;
        }
    }
    else if (state.turn == black)
    {
        if (state.can_castle_q
            && board[7][6] == ' '
            && board[7][5] == ' '
            && board[7][4] == ' '
            && !is_attacked_yx(y, x)
            && !is_attacked_yx(7, 4)
            && move_is_safe(QUEENSIDE_BLACK))
        {
            moves[moves_len++] = QUEENSIDE_BLACK;
        }
        if (state.can_castle_k
            && board[7][1] == ' '
            && board[7][2] == ' '
            && !is_attacked_yx(y, x)
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

bool is_move_equal(move_t a, move_t b)
{
    return a.piece == b.piece
        && a.from_x == b.from_x
        && a.from_y == b.from_y
        && a.to_x == b.to_x
        && a.to_y == b.to_y
        && a.target == b.target
        && a.promotion_piece == b.promotion_piece;
}

move_value_t alpha_beta(int depth, int beta, int alpha, move_t move)
{
    move_value_t return_move ={move, 0};
    if (depth == 0)
    {
        move_value_t temp_move ={move, rate(board, 0, 0)};
        return temp_move;
    }

    move_t *moves = malloc(MAX_LIST_LENGTH * sizeof(move_t));
    int number_of_moves = generate_moves(moves);

    if (number_of_moves == 0)
    {
        move_value_t temp_move ={move, rate(board, number_of_moves, depth)};
        return temp_move;
    }

    for (int i = 0; i < number_of_moves; i++)
    {
        state_t old_state = state;
        make_move(moves[i]);
        rotate_board();
        move_value_t next_move = alpha_beta(depth - 1, beta, alpha, moves[i]);
        int eval = next_move.value;
        rotate_board();
        undo_move(moves[i], old_state);

        if (state.turn == white)
        {
            if (eval <= beta)
            {
                beta = eval;
                if (depth == max_depth)
                {
                    return_move = next_move;
                }
            }
        }
        else
        {
            if (eval > alpha)
            {
                alpha = eval;
                if (depth == max_depth) {
                    return_move = next_move;
                }
            }
        }
        if (alpha >= beta)
        {
            if (state.turn == white)
            {
                return_move.value = beta;
            } else
            {
                return_move.value = alpha;
            }
            return return_move;
        }
    }
    if (state.turn == white)
    {
        return_move.value = beta;
    } else
    {
        return_move.value = alpha;
    }
    return return_move;
}

long perft(int depth, int max_depth)
{
    if (depth == 0)
    {
        return 1;
    }

    move_t *moves = malloc(MAX_LIST_LENGTH * sizeof(move_t));
    int number_of_moves = generate_moves(moves);

    long count = 0;
    for (int i = 0; i < number_of_moves; i++)
    {
        // move_t m ={'B', 3, 0, 2, 1, ' '};
        // if (state.turn == black && is_move_equal(last_move, m))
        // {
        //     print_board(board, state);
        //     printf("turn = %d depth = %d ", state.turn, depth);
        //     log_move(rotate_move(moves[i]));
        // }
        // for (int i = 0; i < max_depth - depth; i++)
        // {
        //     printf("    ");
        // }
        // if (state.turn == white)
        // {
        //     log_move(moves[i]);
        // }
        // else
        // {
        //     log_move(rotate_move(moves[i]));
        // }
        
        // char old_board[8][8];
        // memcpy(old_board, board, 64);
        long old_count = count;

        state_t old_state = state;
        make_move(moves[i]);
        rotate_board();
        count += perft(depth - 1, max_depth);
        rotate_board();
        undo_move(moves[i], old_state);

        // if (depth == max_depth)
        // {
        //     // Divide
        //     printf("%d ", count - old_count);
        //     log_move(moves[i]);
        // }

        // if (memcmp(board, old_board, 64) != 0)
        // {
        //     puts("Old board:");
        //     print_board(old_board, old_state);
        //     puts("New board:");
        //     print_board(board, state);
        //     printf("ERROR: board not identical after undo\n turn = %d depth = %d move = ", state.turn, depth);
        //     log_move(moves[i]);
        //     exit(EXIT_FAILURE);
        // }
    }
    return count;
}

void run_tests()
{
    char *tests[26];
    int test_depths[13] ={6, 6, 6, 6, 4, 4, 6, 5, 6, 6, 6, 7, 4};
    long correct_result[13] ={824064, 1440467, 661072, 803711, 1274206, 1720476, 3821001, 1004658, 217342, 92683, 2217, 567584, 23527};
    // avoid illegal en passant capture: pass
    tests[0] = "8/5bk1/8/2Pp4/8/1K6/8/8 w - d6 0 1"; // 6 = 824064
    tests[1] = "8/8/1k6/8/2pP4/8/5BK1/8 b - d3 0 1"; // 6 = 824064
    // en passant capture checks opponent: pass
    tests[2] = "8/8/1k6/2b5/2pP4/8/5K2/8 b - d3 0 1"; // 6 = 1440467
    tests[3] = "8/5k2/8/2Pp4/2B5/1K6/8/8 w - d6 0 1"; // 6 = 1440467
    // "short castling gives check: pass
    tests[4] = "5k2/8/8/8/8/8/8/4K2R w K - 0 1"; // 6 = 661072
    tests[5] = "4k2r/8/8/8/8/8/8/5K2 b k - 0 1"; // 6 = 661072
    // long castling gives check: pass
    tests[6] = "3k4/8/8/8/8/8/8/R3K3 w Q - 0 1"; // 6 = 803711
    tests[7] = "r3k3/8/8/8/8/8/8/3K4 b q - 0 1"; // 6 = 803711
    // castling (including losing cr due to rook capture): pass
    tests[8] = "r3k2r/1b4bq/8/8/8/8/7B/R3K2R w KQkq - 0 1"; // 4 = 1274206
    tests[9] = "r3k2r/7b/8/8/8/8/1B4BQ/R3K2R b KQkq - 0 1"; // 4 = 1274206
    // castling prevented: pass
    tests[10] = "r3k2r/8/3Q4/8/8/5q2/8/R3K2R b KQkq - 0 1"; // 4 = 1720476
    tests[11] = "r3k2r/8/5Q2/8/8/3q4/8/R3K2R w KQkq - 0 1"; // 4 = 1720476
    // promote out of check: pass
    tests[12] = "2K2r2/4P3/8/8/8/8/8/3k4 w - - 0 1"; // 6 = 3821001
    tests[13] = "3K4/8/8/8/8/8/4p3/2k2R2 b - - 0 1"; // 6 = 3821001
    // discovered check: pass
    tests[14] = "8/8/1P2K3/8/2n5/1q6/8/5k2 b - - 0 1"; // 5 = 1004658
    tests[15] = "5K2/8/1Q6/2N5/8/1p2k3/8/8 w - - 0 1"; // 5 = 1004658
    // promote to give check: pass
    tests[16] = "4k3/1P6/8/8/8/8/K7/8 w - - 0 1"; // 6 = 217342
    tests[17] = "8/k7/8/8/8/8/1p6/4K3 b - - 0 1"; // 6 = 217342
    // underpromote to check: pass
    tests[18] = "8/P1k5/K7/8/8/8/8/8 w - - 0 1"; // 6 = 92683
    tests[19] = "8/8/8/8/8/k7/p1K5/8 b - - 0 1"; // 6 = 92683
    // self stalemate: pass
    tests[20] = "K1k5/8/P7/8/8/8/8/8 w - - 0 1"; // 6 = 2217
    tests[21] = "8/8/8/8/8/p7/8/k1K5 b - - 0 1"; // 6 = 2217
    // stalemate/checkmate: pass
    tests[22] = "8/k1P5/8/1K6/8/8/8/8 w - - 0 1"; // 7 = 567584
    tests[23] = "8/8/8/8/1k6/8/K1p5/8 b - - 0 1"; // 7 = 567584
    // double check: pass
    tests[24] = "8/8/2k5/5q2/5n2/8/5K2/8 b - - 0 1"; // 4 = 23527
    tests[25] = "8/5k2/8/5N2/5Q2/2K5/8/8 w - - 0 1"; // 4 = 23527

    for (int i = 0; i < 26; i++)
    {
        printf("Test %d, depth %d, %s\n", i, test_depths[i/2], tests[i]);
        initialise_board(tests[i]);
        if (i == 0 || i == 3)
        {
            state.ep_x = 3;
        }
        else if (i == 1 || i == 2)
        {
            state.ep_x = 4;
        }
        long node_count = perft(test_depths[i/2], test_depths[i/2]);
        if (node_count == correct_result[i/2])
        {
            printf("Passed: %d\n", node_count);
        }
        else
        {
            printf("FAILED: %d instead of %d\n", node_count, correct_result[i/2]);
        }
    }
    exit(EXIT_SUCCESS);
}

int main()
{
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

    initialise_board(STARTING_POSITION);
    while (true)
    {
        print_board(board, state);
        move_t move;
        do
        {
            move = get_move_pieces(get_user_move(state.turn));
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
                print_board(board, state);
                puts("Checkmate!");
                return 0;
            }
            else
            {
                rotate_board();
                print_board(board, state);
                puts("Stalemate");
                return 0;
            }
        }
        move_t computer_move = alpha_beta(max_depth, 1000000, -1000000, BLANK_MOVE).move;
        log_move(computer_move);
        make_move(computer_move);
        rotate_board();
    }
}
