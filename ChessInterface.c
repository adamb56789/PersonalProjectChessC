#include "ChessInterface.h"

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

move_t get_move_from_location(char board[8][8], int from_y, int from_x, int to_y, int to_x)
{
    move_t move;
    // calculate locations in 0-63 form
    move.from = from_y * 8 + from_x;
    move.to = to_y * 8 + to_x;
    move.piece = *(*board + move.from);
    move.target = *(*board + move.to);

    // If castling, the moving piece is C for kingside or c for queenside
    // If promotion, the mobing piece is ^

    if (move.piece == 'K' && move.from == 60) // Castling
    {
        if (move.to == 62) // Kingside
        {
            move.piece = 'C';
        }
        else if (move.to == 58) // Queenside
        {
            move.piece = 'c';
        }
    }
    else if (move.piece == 'P' && move.to < 8) // Promotion
    {
        move.piece = '^';
    }
    return move;
}

move_t get_user_move(char board[8][8], state_t state)
{
    const char *LETTERLINE = "    a   b   c   d   e   f   g   h";
    const char *TOPLINE = "  \u250C\u2500\u2500\u2500\u252C\u2500\u2500\u2500\u252C\u2500\u2500\u2500\u252C\u2500\u2500\u2500\u252C\u2500\u2500\u2500\u252C\u2500\u2500\u2500\u252C\u2500\u2500\u2500\u252C\u2500\u2500\u2500\u2510";
    const char *MIDLINE = "  \u251C\u2500\u2500\u2500\u253C\u2500\u2500\u2500\u253C\u2500\u2500\u2500\u253C\u2500\u2500\u2500\u253C\u2500\u2500\u2500\u253C\u2500\u2500\u2500\u253C\u2500\u2500\u2500\u253C\u2500\u2500\u2500\u2524";
    const char *BOTLINE = "  \u2514\u2500\u2500\u2500\u2534\u2500\u2500\u2500\u2534\u2500\u2500\u2500\u2534\u2500\u2500\u2500\u2534\u2500\u2500\u2500\u2534\u2500\u2500\u2500\u2534\u2500\u2500\u2500\u2534\u2500\u2500\u2500\u2518";

    puts(LETTERLINE);
    puts(TOPLINE);
    for (size_t i = 0; i < 8; i++)
    {
        printf("%d \u2502", 8 - i);
        for (size_t j = 0; j < 8; j++)
        {
            printf(" %s \u2502", to_chess_symbol(board[i][j]));
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
    const size_t INPUT_SIZE = 8;
    char input[INPUT_SIZE];

    bool valid = true;
    bool has_space;
    do
    {
        printf("Enter move: ");
        fgets(input, sizeof(input), stdin);

        if (strcmp(input, "exit\n") == 0)
        {
            exit(EXIT_SUCCESS);
        }
        

        // check format, accept both "e2e4" and "e2 e4"
        has_space = input[2] == ' ';
        valid = 'a' <= input[0] && input[0] <= 'h';
        valid &= '1' <= input[1] && input[1] <= '8';
        valid &= 'a' <= input[2 + has_space] && input[2 + has_space] <= 'h';
        valid &= '1' <= input[3 + has_space] && input[3 + has_space] <= '8';
        if (!valid)
        {
            puts("Invalid input, should look like e2e4 or e2 e4. Type exit to exit.");
        }
    } while (!valid);

    int from_y = 8 - (input[1] - 48);
    int from_x = input[0] - 97;
    int to_y = 8 - (input[3 + has_space] - 48);
    int to_x = input[2 + has_space] - 97;
    return get_move_from_location(board, from_y, from_x, to_y, to_x);
}
