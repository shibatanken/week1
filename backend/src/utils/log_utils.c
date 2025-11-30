#include "log_utils.h"
#include <stdio.h>
#include <stdlib.h>

void log_to_file(const char *log_content, const char *timestamp)
{
    FILE *file = fopen("log.txt", "a");
    if (file == NULL)
    {
        fprintf(stderr, "Failed to open log file\n");
        return;
    }

    fprintf(file, "%s - %s\n", timestamp, log_content);
    fclose(file);
}