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
    printf("%c %c%d %c%d %c %c\n", move.piece, move.from_x + 97, 8 - move.from_y,
        move.to_x + 97, 8 - move.to_y, move.target, move.involved_piece);
    // TODO log to files
}
