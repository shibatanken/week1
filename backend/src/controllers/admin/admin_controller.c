#include "admin_controller.h"
#include "../../services/admin/admin_service.h"
#include "../../utils/json_utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

void handle_admin_get_pending_users(int client_socket, ControlMessage *msg)
{
    (void)msg; // unused
    char *result = get_pending_users();
    char *response;

    if (result == NULL) {
        response = strdup("DATA JSON PENDING_USERS\n{\"data\": []}");
    } else {
        size_t response_size = strlen(result) + 64;
        response = (char *)malloc(response_size);
        snprintf(response, response_size, "DATA JSON PENDING_USERS\n{\"data\": %s}", result);
        free(result);
    }

    write(client_socket, response, strlen(response));
    close(client_socket);
    free(response);
}

void handle_admin_approve_user(int client_socket, ControlMessage *msg)
{
    KeyValuePair pairs[10];
    int pair_count = parse_json(msg->body, pairs, 1);
    int user_id = -1;
    for (int i = 0; i < pair_count; i++) {
        if (strcmp(pairs[i].key, "user_id") == 0) user_id = atoi(pairs[i].value);
    }

    int result = approve_user(user_id);
    char response[2048];
    char timestamp[50];
    time_t now = time(NULL);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%S", localtime(&now));

    if (result == 0) {
        snprintf(response, sizeof(response), "NOTIFICATION APPROVE_USER_FAILURE %s\n{\"message\": \"Failed\"}", timestamp);
    } else {
        snprintf(response, sizeof(response), "NOTIFICATION APPROVE_USER_SUCCESS %s\n{\"user_id\": %d}", timestamp, user_id);
    }

    write(client_socket, response, strlen(response));
    close(client_socket);
}

void handle_admin_reject_user(int client_socket, ControlMessage *msg)
{
    KeyValuePair pairs[10];
    int pair_count = parse_json(msg->body, pairs, 1);
    int user_id = -1;
    for (int i = 0; i < pair_count; i++) {
        if (strcmp(pairs[i].key, "user_id") == 0) user_id = atoi(pairs[i].value);
    }

    int result = reject_user(user_id);
    char response[2048];
    char timestamp[50];
    time_t now = time(NULL);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%S", localtime(&now));

    if (result == 0) {
        snprintf(response, sizeof(response), "NOTIFICATION REJECT_USER_FAILURE %s\n{\"message\": \"Failed\"}", timestamp);
    } else {
        snprintf(response, sizeof(response), "NOTIFICATION REJECT_USER_SUCCESS %s\n{\"user_id\": %d}", timestamp, user_id);
    }

    write(client_socket, response, strlen(response));
    close(client_socket);
}

void handle_admin_get_all_users(int client_socket, ControlMessage *msg)
{
    (void)msg;
    char *result = get_all_users();
    char *response;

    if (result == NULL) {
        response = strdup("DATA JSON ALL_USERS\n{\"data\": []}");
    } else {
        size_t response_size = strlen(result) + 64;
        response = (char *)malloc(response_size);
        snprintf(response, response_size, "DATA JSON ALL_USERS\n{\"data\": %s}", result);
        free(result);
    }

    write(client_socket, response, strlen(response));
    close(client_socket);
    free(response);
}

void handle_admin_add_user(int client_socket, ControlMessage *msg)
{
    KeyValuePair pairs[10];
    int pair_count = parse_json(msg->body, pairs, 10);

    char email[256] = "", password[256] = "", name[256] = "", role[32] = "student";

    for (int i = 0; i < pair_count; i++) {
        if (strcmp(pairs[i].key, "email") == 0) strncpy(email, pairs[i].value, sizeof(email) - 1);
        else if (strcmp(pairs[i].key, "password") == 0) strncpy(password, pairs[i].value, sizeof(password) - 1);
        else if (strcmp(pairs[i].key, "name") == 0) strncpy(name, pairs[i].value, sizeof(name) - 1);
        else if (strcmp(pairs[i].key, "role") == 0) strncpy(role, pairs[i].value, sizeof(role) - 1);
    }

    int user_id = admin_add_user(email, password, name, role);
    char response[2048];
    char timestamp[50];
    time_t now = time(NULL);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%S", localtime(&now));

    if (user_id == 0) {
        snprintf(response, sizeof(response), "NOTIFICATION ADMIN_ADD_USER_FAILURE %s\n{\"message\": \"Failed\"}", timestamp);
    } else {
        snprintf(response, sizeof(response), "NOTIFICATION ADMIN_ADD_USER_SUCCESS %s\n{\"user_id\": %d}", timestamp, user_id);
    }

    write(client_socket, response, strlen(response));
    close(client_socket);
}

void handle_admin_update_user(int client_socket, ControlMessage *msg)
{
    KeyValuePair pairs[10];
    int pair_count = parse_json(msg->body, pairs, 10);

    int user_id = -1;
    char email[256] = "", name[256] = "", role[32] = "", status[32] = "";

    for (int i = 0; i < pair_count; i++) {
        if (strcmp(pairs[i].key, "user_id") == 0) user_id = atoi(pairs[i].value);
        else if (strcmp(pairs[i].key, "email") == 0) strncpy(email, pairs[i].value, sizeof(email) - 1);
        else if (strcmp(pairs[i].key, "name") == 0) strncpy(name, pairs[i].value, sizeof(name) - 1);
        else if (strcmp(pairs[i].key, "role") == 0) strncpy(role, pairs[i].value, sizeof(role) - 1);
        else if (strcmp(pairs[i].key, "status") == 0) strncpy(status, pairs[i].value, sizeof(status) - 1);
    }

    int result = admin_update_user(user_id, email, name, role, status);
    char response[2048];
    char timestamp[50];
    time_t now = time(NULL);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%S", localtime(&now));

    if (result == 0) {
        snprintf(response, sizeof(response), "NOTIFICATION ADMIN_UPDATE_USER_FAILURE %s\n{\"message\": \"Failed\"}", timestamp);
    } else {
        snprintf(response, sizeof(response), "NOTIFICATION ADMIN_UPDATE_USER_SUCCESS %s\n{\"user_id\": %d}", timestamp, user_id);
    }

    write(client_socket, response, strlen(response));
    close(client_socket);
}

void handle_admin_delete_user(int client_socket, ControlMessage *msg)
{
    KeyValuePair pairs[10];
    int pair_count = parse_json(msg->body, pairs, 1);
    int user_id = -1;
    for (int i = 0; i < pair_count; i++) {
        if (strcmp(pairs[i].key, "user_id") == 0) user_id = atoi(pairs[i].value);
    }

    int result = admin_delete_user(user_id);
    char response[2048];
    char timestamp[50];
    time_t now = time(NULL);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%S", localtime(&now));

    if (result == 0) {
        snprintf(response, sizeof(response), "NOTIFICATION ADMIN_DELETE_USER_FAILURE %s\n{\"message\": \"Failed\"}", timestamp);
    } else {
        snprintf(response, sizeof(response), "NOTIFICATION ADMIN_DELETE_USER_SUCCESS %s\n{\"user_id\": %d}", timestamp, user_id);
    }

    write(client_socket, response, strlen(response));
    close(client_socket);
}

