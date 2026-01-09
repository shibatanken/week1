#include "appeal_controller.h"
#include "../../services/appeal/appeal_service.h"
#include "../../utils/json_utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

void handle_submit_appeal(int client_socket, ControlMessage *msg)
{
    KeyValuePair pairs[10];
    int pair_count = parse_json(msg->body, pairs, 10);

    int submission_id = -1, question_id = -1, user_id = -1;
    char reason[1024] = "";

    for (int i = 0; i < pair_count; i++) {
        if (strcmp(pairs[i].key, "submission_id") == 0) submission_id = atoi(pairs[i].value);
        else if (strcmp(pairs[i].key, "question_id") == 0) question_id = atoi(pairs[i].value);
        else if (strcmp(pairs[i].key, "user_id") == 0) user_id = atoi(pairs[i].value);
        else if (strcmp(pairs[i].key, "reason") == 0) strncpy(reason, pairs[i].value, sizeof(reason) - 1);
    }

    int appeal_id = submit_appeal(submission_id, question_id, user_id, reason);
    char response[2048];
    char timestamp[50];
    time_t now = time(NULL);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%S", localtime(&now));

    if (appeal_id == 0) {
        snprintf(response, sizeof(response), "NOTIFICATION SUBMIT_APPEAL_FAILURE %s\n{\"message\": \"Failed to submit appeal\"}", timestamp);
    } else {
        snprintf(response, sizeof(response), "NOTIFICATION SUBMIT_APPEAL_SUCCESS %s\n{\"appeal_id\": %d}", timestamp, appeal_id);
    }

    write(client_socket, response, strlen(response));
    close(client_socket);
}

void handle_get_my_appeals(int client_socket, ControlMessage *msg)
{
    KeyValuePair pairs[10];
    int pair_count = parse_json(msg->body, pairs, 1);
    int user_id = -1;
    for (int i = 0; i < pair_count; i++) {
        if (strcmp(pairs[i].key, "user_id") == 0) user_id = atoi(pairs[i].value);
    }

    char *result = get_my_appeals(user_id);
    char *response;

    if (result == NULL) {
        response = strdup("DATA JSON MY_APPEALS\n{\"data\": []}");
    } else {
        size_t response_size = strlen(result) + 64;
        response = (char *)malloc(response_size);
        snprintf(response, response_size, "DATA JSON MY_APPEALS\n{\"data\": %s}", result);
        free(result);
    }

    write(client_socket, response, strlen(response));
    close(client_socket);
    free(response);
}

void handle_get_appeals_for_teacher(int client_socket, ControlMessage *msg)
{
    KeyValuePair pairs[10];
    int pair_count = parse_json(msg->body, pairs, 1);
    int teacher_id = -1;
    for (int i = 0; i < pair_count; i++) {
        if (strcmp(pairs[i].key, "teacher_id") == 0) teacher_id = atoi(pairs[i].value);
    }

    char *result = get_appeals_for_teacher(teacher_id);
    char *response;

    if (result == NULL) {
        response = strdup("DATA JSON APPEALS_FOR_TEACHER\n{\"data\": []}");
    } else {
        size_t response_size = strlen(result) + 64;
        response = (char *)malloc(response_size);
        snprintf(response, response_size, "DATA JSON APPEALS_FOR_TEACHER\n{\"data\": %s}", result);
        free(result);
    }

    write(client_socket, response, strlen(response));
    close(client_socket);
    free(response);
}

void handle_review_appeal(int client_socket, ControlMessage *msg)
{
    KeyValuePair pairs[10];
    int pair_count = parse_json(msg->body, pairs, 10);

    int appeal_id = -1;
    char status[32] = "", response_text[1024] = "";
    double score_adj = 0;

    for (int i = 0; i < pair_count; i++) {
        if (strcmp(pairs[i].key, "appeal_id") == 0) appeal_id = atoi(pairs[i].value);
        else if (strcmp(pairs[i].key, "status") == 0) strncpy(status, pairs[i].value, sizeof(status) - 1);
        else if (strcmp(pairs[i].key, "response") == 0) strncpy(response_text, pairs[i].value, sizeof(response_text) - 1);
        else if (strcmp(pairs[i].key, "score_adjustment") == 0) score_adj = atof(pairs[i].value);
    }

    int result = review_appeal(appeal_id, status, response_text, score_adj);
    char response[2048];
    char timestamp[50];
    time_t now = time(NULL);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%S", localtime(&now));

    if (result == 0) {
        snprintf(response, sizeof(response), "NOTIFICATION REVIEW_APPEAL_FAILURE %s\n{\"message\": \"Failed to review appeal\"}", timestamp);
    } else {
        snprintf(response, sizeof(response), "NOTIFICATION REVIEW_APPEAL_SUCCESS %s\n{\"appeal_id\": %d, \"status\": \"%s\"}", timestamp, appeal_id, status);
    }

    write(client_socket, response, strlen(response));
    close(client_socket);
}

