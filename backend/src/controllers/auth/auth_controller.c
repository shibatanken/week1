#include "auth_controller.h"
#include "../../data_structures/index.h"
#include "../../services/service.h"
#include "../../utils/json_utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

void handle_login(int client_socket, ControlMessage *msg)
{
    KeyValuePair pairs[10];
    int pair_count = parse_json(msg->body, pairs, 2);

    char email[256] = "", password[256] = "";
    for (int i = 0; i < pair_count; i++)
    {
        if (strcmp(pairs[i].key, "email") == 0)
        {
            strncpy(email, pairs[i].value, sizeof(email) - 1);
        }
        else if (strcmp(pairs[i].key, "password") == 0)
        {
            strncpy(password, pairs[i].value, sizeof(password) - 1);
        }
    }

    int user_id = login(email, password);
    char response[2048];
    char timestamp[50];

    time_t now = time(NULL);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%S", localtime(&now));

    if (user_id != -1)
    {
        snprintf(response, sizeof(response), "NOTIFICATION LOGIN_SUCCESS %s\n{\"user_id\": %d}", timestamp, user_id);
    }
    // {\" \": \" \"}
    else
    {
        snprintf(response, sizeof(response), "NOTIFICATION LOGIN_FAILURE %s\n{\"message\": \"User not found or wrong password\"}", timestamp);
    }

    write(client_socket, response, strlen(response));
    close(client_socket);
}

void handle_signup(int client_socket, ControlMessage *msg)
{
    KeyValuePair pairs[10];
    int pair_count = parse_json(msg->body, pairs, 4);

    char email[256] = "", password[256] = "", username[256] = "", dob[256] = "";
    for (int i = 0; i < pair_count; i++)
    {
        if (strcmp(pairs[i].key, "email") == 0)
        {
            strncpy(email, pairs[i].value, sizeof(email) - 1);
        }
        else if (strcmp(pairs[i].key, "password") == 0)
        {
            strncpy(password, pairs[i].value, sizeof(password) - 1);
        }
        else if (strcmp(pairs[i].key, "name") == 0)
        {
            strncpy(username, pairs[i].value, sizeof(username) - 1);
        }
        else if (strcmp(pairs[i].key, "dob") == 0)
        {
            strncpy(dob, pairs[i].value, sizeof(dob) - 1);
        }
    }

    printf("email: %s, password: %s, username: %s, dob: %s\n", email, password, username, dob);

    int result = signup(email, password, username, dob);

    char response[2048];
    char timestamp[50];
    time_t now = time(NULL);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%S", localtime(&now));

    if (result)
    {
        snprintf(response, sizeof(response), "NOTIFICATION SIGN_UP_SUCCESS %s", timestamp);
    }
    else
    {
        snprintf(response, sizeof(response), "NOTIFICATION SIGN_UP_FAILURE %s", timestamp);
    }

    write(client_socket, response, strlen(response));
    close(client_socket);
}
