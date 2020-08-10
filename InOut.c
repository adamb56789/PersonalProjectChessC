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

void log_move(char *filename, move_t move)
{
    printf("%c %02d %02d %d", move.piece, move.from, move.to, move.target);
    // TODO
}
