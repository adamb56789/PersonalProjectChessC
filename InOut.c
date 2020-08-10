#include "InOut.h"

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

void log_move(char *filename, char *move)
{
    puts(move);
    // TODO
}
