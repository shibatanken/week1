#include "question_controller.h"
#include "../../data_structures/index.h"
#include "../../services/service.h"
#include "../../utils/json_utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <cjson/cJSON.h>
#include <time.h>

void handle_add_question(int client_socket, ControlMessage *msg)
{
    KeyValuePair pairs[10];
    int pair_count = parse_json(msg->body, pairs, 10);

    int room_id = -1, difficulty = -1;
    char content[512] = "";
    cJSON *answer_list = NULL;

    for (int i = 0; i < pair_count; i++)
    {
        if (strcmp(pairs[i].key, "room_id") == 0)
        {
            room_id = atoi(pairs[i].value);
        }
        else if (strcmp(pairs[i].key, "content") == 0)
        {
            strncpy(content, pairs[i].value, sizeof(content) - 1);
        }
        else if (strcmp(pairs[i].key, "difficulty") == 0)
        {
            difficulty = atoi(pairs[i].value);
        }
        else if (strcmp(pairs[i].key, "answer_list") == 0)
        {
            answer_list = cJSON_Parse(pairs[i].value);
        }
    }

    int question_id;
    int result = add_question(room_id, content, difficulty, &question_id);
    char response[2048];
    char timestamp[50];
    time_t now = time(NULL);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%S", localtime(&now));

    if (result == 0)
    {
        snprintf(response, sizeof(response), "NOTIFICATION ADD_QUESTION_FAILURE %s\n{\"message\": \"Failed to add question\"}", timestamp);
    }
    else
    {
        if (answer_list != NULL)
        {
            cJSON *answer;
            cJSON_ArrayForEach(answer, answer_list)
            {
                cJSON *answer_content = cJSON_GetObjectItem(answer, "content");
                cJSON *is_true = cJSON_GetObjectItem(answer, "is_true");

                if (cJSON_IsString(answer_content) && cJSON_IsBool(is_true))
                {
                    add_answer(question_id, answer_content->valuestring, is_true->valueint);
                }
            }
        }

        snprintf(response, sizeof(response), "NOTIFICATION ADD_QUESTION_SUCCESS %s\n{\"message\": \"Question added successfully\"}", timestamp);
    }

    write(client_socket, response, strlen(response));
    close(client_socket);

    if (answer_list != NULL)
    {
        cJSON_Delete(answer_list);
    }
}