#include "InOut.h"
#include "data_types.h"

char* get_settings()
{
    FILE *fp;
    char *buff = malloc(32 * sizeof(char));

    fp = fopen("Settings.txt", "r");
    // Discard first line
    fgets(buff, 32, (FILE*)fp);
    fgets(buff, 32, (FILE*)fp);

    fclose(fp);
    return buff;
}

void log_move(move_t move)
{
    // int from_y = 8 - (input[1] - 48);
    // int from_x = input[0] - 97;
    // int to_y = 8 - (input[3 + has_space] - 48);
    // int to_x = input[2 + has_space] - 97;
    printf("%c %c%d %c%d %c\n", move.piece, move.from_x + 97, 8 - move.from_y,
    move.to_x + 97, 8 - move.to_y, move.target);
    // TODO
}
