#include "statistics_controller.h"
#include "../../services/statistics/statistics_service.h"
#include "../../utils/json_utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

void handle_get_exam_statistics(int client_socket, ControlMessage *msg)
{
    KeyValuePair pairs[10];
    int pair_count = parse_json(msg->body, pairs, 1);
    int exam_id = -1;
    for (int i = 0; i < pair_count; i++) {
        if (strcmp(pairs[i].key, "exam_id") == 0) exam_id = atoi(pairs[i].value);
    }

    char *result = get_exam_statistics(exam_id);
    char *response;

    if (result == NULL) {
        response = strdup("DATA JSON EXAM_STATISTICS\n{}");
    } else {
        size_t response_size = strlen(result) + 64;
        response = (char *)malloc(response_size);
        snprintf(response, response_size, "DATA JSON EXAM_STATISTICS\n%s", result);
        free(result);
    }

    write(client_socket, response, strlen(response));
    close(client_socket);
    free(response);
}

void handle_get_class_statistics(int client_socket, ControlMessage *msg)
{
    KeyValuePair pairs[10];
    int pair_count = parse_json(msg->body, pairs, 1);
    int class_id = -1;
    for (int i = 0; i < pair_count; i++) {
        if (strcmp(pairs[i].key, "class_id") == 0) class_id = atoi(pairs[i].value);
    }

    char *result = get_class_statistics(class_id);
    char *response;

    if (result == NULL) {
        response = strdup("DATA JSON CLASS_STATISTICS\n{}");
    } else {
        size_t response_size = strlen(result) + 64;
        response = (char *)malloc(response_size);
        snprintf(response, response_size, "DATA JSON CLASS_STATISTICS\n%s", result);
        free(result);
    }

    write(client_socket, response, strlen(response));
    close(client_socket);
    free(response);
}

