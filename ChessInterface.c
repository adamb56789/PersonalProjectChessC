#include "ChessInterface.h"

// For printing the board
const char *LETTERLINE = "    a   b   c   d   e   f   g   h";
const char *TOPLINE = "  \u250C\u2500\u2500\u2500\u252C\u2500\u2500\u2500\u252C\u2500\u2500\u2500\u252C\u2500\u2500\u2500\u252C\u2500\u2500\u2500\u252C\u2500\u2500\u2500\u252C\u2500\u2500\u2500\u252C\u2500\u2500\u2500\u2510";
const char *MIDLINE = "  \u251C\u2500\u2500\u2500\u253C\u2500\u2500\u2500\u253C\u2500\u2500\u2500\u253C\u2500\u2500\u2500\u253C\u2500\u2500\u2500\u253C\u2500\u2500\u2500\u253C\u2500\u2500\u2500\u253C\u2500\u2500\u2500\u2524";
const char *BOTLINE = "  \u2514\u2500\u2500\u2500\u2534\u2500\u2500\u2500\u2534\u2500\u2500\u2500\u2534\u2500\u2500\u2500\u2534\u2500\u2500\u2500\u2534\u2500\u2500\u2500\u2534\u2500\u2500\u2500\u2534\u2500\u2500\u2500\u2518";

// The unicode chess symbols are 3 bytes
char* to_chess_symbol(char c)
{
    switch (c)
    {
    case 'K': return "\u2654"; break;
    case 'Q': return "\u2655"; break;
    case 'R': return "\u2656"; break;
    case 'B': return "\u2657"; break;
    case 'N': return "\u2658"; break;
    case 'P': return "\u2659"; break;
    case 'k': return "\u265A"; break;
    case 'q': return "\u265B"; break;
    case 'r': return "\u265C"; break;
    case 'b': return "\u265D"; break;
    case 'n': return "\u265E"; break;
    case 'p': return "\u265F"; break;
    case ' ': return " "; break;
    default:
        printf("Cannot print '%c' which is not a valid piece", c);
        exit(EXIT_FAILURE);
        break;
    }
}

char* to_chess_symbol_inverted_colour(char c)
{
    switch (c)
    {
    case 'k': return "\u2654"; break;
    case 'q': return "\u2655"; break;
    case 'r': return "\u2656"; break;
    case 'b': return "\u2657"; break;
    case 'n': return "\u2658"; break;
    case 'p': return "\u2659"; break;
    case 'K': return "\u265A"; break;
    case 'Q': return "\u265B"; break;
    case 'R': return "\u265C"; break;
    case 'B': return "\u265D"; break;
    case 'N': return "\u265E"; break;
    case 'P': return "\u265F"; break;
    case ' ': return " "; break;
    default:
        printf("Cannot print '%x' which is not a valid piece", c);
        exit(EXIT_FAILURE);
        break;
    }
}

move_t to_move(char board[8][8], int from_y, int from_x, int to_y, int to_x)
{
    // If anything is out of bounds then something went wrong
    // TODO remove this for effeciency
    if (from_y < 0 || 7 < from_y || from_x < 0 || 7 < from_x ||
        to_y < 0 || 7 < to_y || to_x < 0 || 7 < to_x)
    {
        printf("Out of bounds [%d][%d] [%d][%d]\n", from_y,  from_x, to_y, to_x);
        exit(EXIT_FAILURE);
    }

    // This move's piece data may be incorrect (missing castling etc)
    move_t move ={ board[from_y][from_x], from_y, from_x, to_y, to_x, board[to_y][to_x] };

    return move;
}

void print_board(char board[8][8], state_t game)
{
    puts(LETTERLINE);
    puts(TOPLINE);
    for (size_t i = 0; i < 8; i++)
    {
        printf("%d \u2502", 8 - i);
        for (size_t j = 0; j < 8; j++)
        {
            if (game.turn == white)
            {
                // Colours are inverted since on a black background,
                // the black pieces appear as solid white
                printf(" %s \u2502", to_chess_symbol_inverted_colour(board[i][j]));
            }
            else
            {
                printf(" %s \u2502", to_chess_symbol(board[i][j]));
            }
            // printf(" %s \u2502", board[i][j]);
        }
        printf("\n");
        if (i != 7)
        {
            puts(MIDLINE);
        }
        else
        {
            puts(BOTLINE);
        }
    }
}

user_move_t get_user_move()
{
    char input[255];
    bool valid = true;
    bool promotion_entered = false;
    do
    {
        printf("Enter move: ");
        fgets(input, sizeof(input), stdin);

        if (strcmp(input, "exit\n") == 0)
        {
            exit(EXIT_SUCCESS);
        }

        valid = 'a' <= input[0] && input[0] <= 'h'
            && '1' <= input[1] && input[1] <= '8'
            && 'a' <= input[2] && input[2] <= 'h'
            && '1' <= input[3] && input[3] <= '8';
        
        if (input[4] != '\n')
        {
            promotion_entered = true;
            valid = valid
                && input[4] == ' '
                && input[6] == '\n'
                && (input[5] == 'q' || input[5] == 'r' || input[5] == 'b' || input[5] == 'n');
        }
        
        if (!valid)
        {
            puts("Invalid input, should look like e2e4 or a7a8 b (promotion to bishop). Type exit to exit.");
        }
    } while (!valid);

    int from_y = 8 - (input[1] - 48);
    int from_x = input[0] - 97;
    int to_y = 8 - (input[3] - 48);
    int to_x = input[2] - 97;
    char promotion_piece = promotion_entered ? input[5] - 32 : ' ';

    // If anything is out of bounds then something went wrong
    // TODO remove this for effeciency
    if (from_y < 0 || 7 < from_y || from_x < 0 || 7 < from_x ||
        to_y < 0 || 7 < to_y || to_x < 0 || 7 < to_x)
    {
        printf("Out of bounds [%d][%d] [%d][%d]\n", from_y,  from_x, to_y, to_x);
        exit(EXIT_FAILURE);
    }

    user_move_t move ={ from_y, from_x, to_y, to_x, promotion_piece };
    return move;
}
