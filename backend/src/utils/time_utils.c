#include "time_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

char *get_current_time()
{
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    char *time_str = (char *)malloc(20 * sizeof(char)); // Allocate memory for the time string
    if (time_str == NULL)
    {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    strftime(time_str, 20, "%Y-%m-%d %H:%M:%S", t); // Format the time string

    return time_str;
}