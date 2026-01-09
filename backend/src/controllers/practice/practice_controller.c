#include "practice_controller.h"
#include "../../services/practice/practice_service.h"
#include "../../utils/json_utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

void handle_start_practice(int client_socket, ControlMessage *msg)
{
    KeyValuePair pairs[10];
    int pair_count = parse_json(msg->body, pairs, 10);

    int user_id = -1, class_id = -1, num_questions = 10;

    for (int i = 0; i < pair_count; i++) {
        if (strcmp(pairs[i].key, "user_id") == 0) user_id = atoi(pairs[i].value);
        else if (strcmp(pairs[i].key, "class_id") == 0) class_id = atoi(pairs[i].value);
        else if (strcmp(pairs[i].key, "num_questions") == 0) num_questions = atoi(pairs[i].value);
    }

    int session_id = start_practice(user_id, class_id, num_questions);
    char response[2048];
    char timestamp[50];
    time_t now = time(NULL);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%S", localtime(&now));

    if (session_id == 0) {
        snprintf(response, sizeof(response), "NOTIFICATION START_PRACTICE_FAILURE %s\n{\"message\": \"Failed\"}", timestamp);
    } else {
        snprintf(response, sizeof(response), "NOTIFICATION START_PRACTICE_SUCCESS %s\n{\"session_id\": %d}", timestamp, session_id);
    }

    write(client_socket, response, strlen(response));
    close(client_socket);
}

void handle_get_practice_questions(int client_socket, ControlMessage *msg)
{
    KeyValuePair pairs[10];
    int pair_count = parse_json(msg->body, pairs, 1);
    int session_id = -1;
    for (int i = 0; i < pair_count; i++) {
        if (strcmp(pairs[i].key, "session_id") == 0) session_id = atoi(pairs[i].value);
    }

    char *result = get_practice_questions(session_id);
    char *response;

    if (result == NULL) {
        response = strdup("DATA JSON PRACTICE_QUESTIONS\n{\"data\": []}");
    } else {
        size_t response_size = strlen(result) + 64;
        response = (char *)malloc(response_size);
        snprintf(response, response_size, "DATA JSON PRACTICE_QUESTIONS\n{\"data\": %s}", result);
        free(result);
    }

    write(client_socket, response, strlen(response));
    close(client_socket);
    free(response);
}

void handle_submit_practice_answer(int client_socket, ControlMessage *msg)
{
    KeyValuePair pairs[10];
    int pair_count = parse_json(msg->body, pairs, 3);

    int session_id = -1, question_id = -1;
    char answer[5] = "";

    for (int i = 0; i < pair_count; i++) {
        if (strcmp(pairs[i].key, "session_id") == 0) session_id = atoi(pairs[i].value);
        else if (strcmp(pairs[i].key, "question_id") == 0) question_id = atoi(pairs[i].value);
        else if (strcmp(pairs[i].key, "answer") == 0) strncpy(answer, pairs[i].value, sizeof(answer) - 1);
    }

    int result = submit_practice_answer(session_id, question_id, answer);
    char response[2048];
    char timestamp[50];
    time_t now = time(NULL);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%S", localtime(&now));

    if (result == 0) {
        snprintf(response, sizeof(response), "NOTIFICATION SUBMIT_PRACTICE_ANSWER_FAILURE %s\n{\"message\": \"Failed\"}", timestamp);
    } else {
        snprintf(response, sizeof(response), "NOTIFICATION SUBMIT_PRACTICE_ANSWER_SUCCESS %s\n{\"question_id\": %d}", timestamp, question_id);
    }

    write(client_socket, response, strlen(response));
    close(client_socket);
}

void handle_finish_practice(int client_socket, ControlMessage *msg)
{
    KeyValuePair pairs[10];
    int pair_count = parse_json(msg->body, pairs, 1);
    int session_id = -1;
    for (int i = 0; i < pair_count; i++) {
        if (strcmp(pairs[i].key, "session_id") == 0) session_id = atoi(pairs[i].value);
    }

    int success = finish_practice(session_id);
    char *result = get_practice_result(session_id);
    char *response;

    if (!success || result == NULL) {
        response = strdup("NOTIFICATION FINISH_PRACTICE_FAILURE\n{\"message\": \"Failed\"}");
    } else {
        size_t response_size = strlen(result) + 128;
        response = (char *)malloc(response_size);
        char timestamp[50];
        time_t now = time(NULL);
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%S", localtime(&now));
        snprintf(response, response_size, "NOTIFICATION FINISH_PRACTICE_SUCCESS %s\n%s", timestamp, result);
        free(result);
    }

    write(client_socket, response, strlen(response));
    close(client_socket);
    free(response);
}

void handle_get_practice_history(int client_socket, ControlMessage *msg)
{
    KeyValuePair pairs[10];
    int pair_count = parse_json(msg->body, pairs, 1);
    int user_id = -1;
    for (int i = 0; i < pair_count; i++) {
        if (strcmp(pairs[i].key, "user_id") == 0) user_id = atoi(pairs[i].value);
    }

    char *result = get_practice_history(user_id);
    char *response;

    if (result == NULL) {
        response = strdup("DATA JSON PRACTICE_HISTORY\n{\"data\": []}");
    } else {
        size_t response_size = strlen(result) + 64;
        response = (char *)malloc(response_size);
        snprintf(response, response_size, "DATA JSON PRACTICE_HISTORY\n{\"data\": %s}", result);
        free(result);
    }

    write(client_socket, response, strlen(response));
    close(client_socket);
    free(response);
}

