#include "room_controller.h"
#include "../../data_structures/index.h"
#include "../../services/service.h"
#include "../../utils/json_utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

void handle_get_room_list(int client_socket, ControlMessage *msg)
{
    char response[8192];
    memset(response, 0, sizeof(response));

    char *result = get_all_room();
    if (result == NULL)
    {
        snprintf(response, sizeof(response), "DATA JSON GET_ROOM_LIST\n{\n\"data\": []\n}");
    }
    else
    {
        snprintf(response, sizeof(response), "DATA JSON GET_ROOM_LIST\n{\n\"data\": %s\n}", result);
    }

    write(client_socket, response, strlen(response));
    close(client_socket);

    if (result != NULL)
    {
        free(result);
    }
}

void handle_create_room(int client_socket, ControlMessage *msg)
{
    KeyValuePair pairs[10];
    int pair_count = parse_json(msg->body, pairs, 10);

    char subject[256] = "", description[256] = "", start[256] = "", end[256] = "";
    int number_of_easy_question = 0, number_of_medium_question = 0, number_of_hard_question = 0, time_limit = 0;

    for (int i = 0; i < pair_count; i++)
    {
        if (strcmp(pairs[i].key, "subject") == 0)
        {
            strncpy(subject, pairs[i].value, sizeof(subject) - 1);
        }
        else if (strcmp(pairs[i].key, "description") == 0)
        {
            strncpy(description, pairs[i].value, sizeof(description) - 1);
        }
        else if (strcmp(pairs[i].key, "number_of_easy_question") == 0)
        {
            number_of_easy_question = atoi(pairs[i].value);
        }
        else if (strcmp(pairs[i].key, "number_of_medium_question") == 0)
        {
            number_of_medium_question = atoi(pairs[i].value);
        }
        else if (strcmp(pairs[i].key, "number_of_hard_question") == 0)
        {
            number_of_hard_question = atoi(pairs[i].value);
        }
        else if (strcmp(pairs[i].key, "time_limit") == 0)
        {
            time_limit = atoi(pairs[i].value);
        }
        else if (strcmp(pairs[i].key, "start") == 0)
        {
            strncpy(start, pairs[i].value, sizeof(start) - 1);
        }
        else if (strcmp(pairs[i].key, "end") == 0)
        {
            strncpy(end, pairs[i].value, sizeof(end) - 1);
        }
    }

    int result = create_room(subject, description, number_of_easy_question, number_of_medium_question, number_of_hard_question, time_limit, start, end);
    char response[2048];
    char timestamp[50];
    time_t now = time(NULL);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%S", localtime(&now));

    if (result == 0)
    {
        snprintf(response, sizeof(response), "NOTIFICATION CREATE_ROOM_FAILURE %s\n{\"message\": \"Failed to create room\"}", timestamp);
    }
    else
    {
        snprintf(response, sizeof(response), "NOTIFICATION CREATE_ROOM_SUCCESS %s\n{\"message\": \"Room created successfully\"}", timestamp);
    }

    write(client_socket, response, strlen(response));
    close(client_socket);
}

void handle_get_room_question(int client_socket, ControlMessage *msg)
{
    KeyValuePair pairs[10];
    int pair_count = parse_json(msg->body, pairs, 1);

    int room_id = -1;
    for (int i = 0; i < pair_count; i++)
    {
        if (strcmp(pairs[i].key, "room_id") == 0)
        {
            room_id = atoi(pairs[i].value);
        }
    }

    char *result = get_room_question(room_id);
    char *response;
    if (result == NULL)
    {
        response = strdup("DATA JSON ROOM_QUESTION\n{\n\"data\": []\n}");
    }
    else
    {
        size_t response_size = strlen(result) + 64;
        response = (char *)malloc(response_size);
        snprintf(response, response_size, "DATA JSON ROOM_QUESTION\n{\n\"data\": %s\n}", result);
    }

    write(client_socket, response, strlen(response));
    close(client_socket);

    free(response);
    if (result != NULL)
    {
        free(result);
    }
}