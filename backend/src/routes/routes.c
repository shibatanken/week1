#include "../data_structures/index.h"
#include "../controllers/controller.h"
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>

pthread_mutex_t lock;

#define PORT 8081
#define BACKLOG 10

typedef struct pthread_arg_t
{
    int new_socket_fd;
    struct sockaddr_in client_address;
} pthread_arg_t;

void *pthread_routine(void *arg);

void handle_control_message(int socket, ControlMessage *msg)
{
    if (strcmp(msg->type, LOGIN) == 0)
    {
        handle_login(socket, msg);
    }
    else if (strcmp(msg->type, SIGN_UP) == 0)
    {
        handle_signup(socket, msg);
    }
    else if (strcmp(msg->type, GET_ROOM_LIST) == 0)
    {
        handle_get_room_list(socket, msg);
    }
    else if (strcmp(msg->type, CREATE_ROOM) == 0)
    {
        handle_create_room(socket, msg);
    }
    else if (strcmp(msg->type, ADD_QUESTION) == 0)
    {
        handle_add_question(socket, msg);
    }
    else if (strcmp(msg->type, GET_ROOM_QUESTION) == 0)
    {
        handle_get_room_question(socket, msg);
    }
}

void handle_data_message(int socket, DataMessage *msg)
{
    // Placeholder
}

void handle_notification_message(int socket, NotificationMessage *msg)
{
    // Placeholder
}

void setup_routes(int server_fd)
{
    struct sockaddr_in address;
    int new_socket;
    pthread_attr_t pthread_attr;
    pthread_arg_t *pthread_arg;
    pthread_t pthread;
    socklen_t client_address_len;

    memset(&address, 0, sizeof address);
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    address.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, BACKLOG) < 0)
    {
        perror("listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("\n Mutex init has failed\n");
        return;
    }

    if (pthread_attr_init(&pthread_attr) != 0)
    {
        perror("pthread_attr_init");
        exit(1);
    }
    if (pthread_attr_setdetachstate(&pthread_attr, PTHREAD_CREATE_DETACHED) != 0)
    {
        perror("pthread_attr_setdetachstate");
        exit(1);
    }

    printf("Server listening on port %d\n", PORT);

    while (1)
    {
        pthread_arg = (pthread_arg_t *)malloc(sizeof *pthread_arg);
        if (!pthread_arg)
        {
            perror("malloc");
            continue;
        }

        client_address_len = sizeof pthread_arg->client_address;
        if ((new_socket = accept(server_fd, (struct sockaddr *)&pthread_arg->client_address, &client_address_len)) < 0)
        {
            perror("accept failed");
            close(server_fd);
            exit(EXIT_FAILURE);
        }

        pthread_arg->new_socket_fd = new_socket;

        if (pthread_create(&pthread, &pthread_attr, pthread_routine, (void *)pthread_arg) != 0)
        {
            perror("pthread_create");
            free(pthread_arg);
            continue;
        }
    }
}

void *pthread_routine(void *arg)
{
    pthread_arg_t *pthread_arg = (pthread_arg_t *)arg;
    int new_socket_fd = pthread_arg->new_socket_fd;

    char buffer[4096];
    memset(buffer, 0, sizeof(buffer));
    read(new_socket_fd, buffer, sizeof(buffer));
    printf("Received message: %s\n", buffer);

    char *header = strtok(buffer, "\n");
    char *body = header + strlen(header) + 1;

    printf("header: %s - body: %s\n", header, body);
    pthread_mutex_lock(&lock);
    if (strncmp(header, "CONTROL", 7) == 0)
    {
        ControlMessage msg;
        sscanf(header, "CONTROL %s", msg.type);
        strncpy(msg.body, body, sizeof(msg.body) - 1);
        handle_control_message(new_socket_fd, &msg);
    }
    else if (strncmp(header, "DATA", 4) == 0)
    {
        DataMessage msg;
        sscanf(header, "DATA %s %s %d", msg.type, msg.data_type, &msg.data_size);
        strncpy(msg.body, body, sizeof(msg.body) - 1);
        handle_data_message(new_socket_fd, &msg);
    }
    else if (strncmp(header, "NOTIFICATION", 12) == 0)
    {
        NotificationMessage msg;
        sscanf(header, "NOTIFICATION %s %s", msg.type, msg.timestamp);
        strncpy(msg.body, body, sizeof(msg.body) - 1);
        handle_notification_message(new_socket_fd, &msg);
    }
    pthread_mutex_unlock(&lock);

    close(new_socket_fd);
    free(pthread_arg);
    return NULL;
}