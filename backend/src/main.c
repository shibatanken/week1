#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "db/connect-db.h"
#include "routes/routes.h"

int main()
{
    int server_fd;
    int opt = 1;

    if (connect_db() != EXIT_SUCCESS)
    {
        return EXIT_FAILURE;
    }
    else
    {
        printf("Connect to db successful\n");
    }

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        close_db();
        return EXIT_FAILURE;
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        perror("setsockopt");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    setup_routes(server_fd);

    close(server_fd);
    close_db();
    return EXIT_SUCCESS;
}