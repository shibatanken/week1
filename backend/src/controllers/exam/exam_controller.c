#include "exam_controller.h"
#include "../../data_structures/index.h"
#include "../../services/service.h"
#include "../../utils/json_utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <cjson/cJSON.h>
#include <time.h>

// Tạo bài kiểm tra mới
void handle_create_exam(int client_socket, ControlMessage *msg)
{
    KeyValuePair pairs[10];
    int pair_count = parse_json(msg->body, pairs, 10);

    char exam_name[256] = "", description[512] = "", start_time[64] = "", end_time[64] = "";
    int class_id = -1, time_limit = 60;

    for (int i = 0; i < pair_count; i++)
    {
        if (strcmp(pairs[i].key, "exam_name") == 0)
        {
            strncpy(exam_name, pairs[i].value, sizeof(exam_name) - 1);
        }
        else if (strcmp(pairs[i].key, "description") == 0)
        {
            strncpy(description, pairs[i].value, sizeof(description) - 1);
        }
        else if (strcmp(pairs[i].key, "class_id") == 0)
        {
            class_id = atoi(pairs[i].value);
        }
        else if (strcmp(pairs[i].key, "time_limit") == 0)
        {
            time_limit = atoi(pairs[i].value);
        }
        else if (strcmp(pairs[i].key, "start_time") == 0)
        {
            strncpy(start_time, pairs[i].value, sizeof(start_time) - 1);
        }
        else if (strcmp(pairs[i].key, "end_time") == 0)
        {
            strncpy(end_time, pairs[i].value, sizeof(end_time) - 1);
        }
    }

    int exam_id = create_exam(exam_name, description, class_id, time_limit, start_time, end_time);
    char response[2048];
    char timestamp[50];
    time_t now = time(NULL);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%S", localtime(&now));

    if (exam_id == 0)
    {
        snprintf(response, sizeof(response),
                 "NOTIFICATION CREATE_EXAM_FAILURE %s\n{\"message\": \"Failed to create exam\"}",
                 timestamp);
    }
    else
    {
        snprintf(response, sizeof(response),
                 "NOTIFICATION CREATE_EXAM_SUCCESS %s\n{\"message\": \"Exam created successfully\", \"exam_id\": %d}",
                 timestamp, exam_id);
    }

    write(client_socket, response, strlen(response));
    close(client_socket);
}

// Lấy danh sách bài kiểm tra trong lớp
void handle_get_exams_in_class(int client_socket, ControlMessage *msg)
{
    KeyValuePair pairs[10];
    int pair_count = parse_json(msg->body, pairs, 1);

    int class_id = -1;
    for (int i = 0; i < pair_count; i++)
    {
        if (strcmp(pairs[i].key, "class_id") == 0)
        {
            class_id = atoi(pairs[i].value);
        }
    }

    char *result = get_exams_in_class(class_id);
    char response[16384];
    memset(response, 0, sizeof(response));

    if (result == NULL)
    {
        snprintf(response, sizeof(response), "DATA JSON EXAMS_IN_CLASS\n{\"data\": []}");
    }
    else
    {
        snprintf(response, sizeof(response), "DATA JSON EXAMS_IN_CLASS\n{\"data\": %s}", result);
        free(result);
    }

    write(client_socket, response, strlen(response));
    close(client_socket);
}

// Lấy chi tiết bài kiểm tra
void handle_get_exam_detail(int client_socket, ControlMessage *msg)
{
    KeyValuePair pairs[10];
    int pair_count = parse_json(msg->body, pairs, 1);

    int exam_id = -1;
    for (int i = 0; i < pair_count; i++)
    {
        if (strcmp(pairs[i].key, "exam_id") == 0)
        {
            exam_id = atoi(pairs[i].value);
        }
    }

    char *result = get_exam_detail(exam_id);
    char response[8192];
    memset(response, 0, sizeof(response));

    if (result == NULL)
    {
        snprintf(response, sizeof(response), "DATA JSON EXAM_DETAIL\n{}");
    }
    else
    {
        snprintf(response, sizeof(response), "DATA JSON EXAM_DETAIL\n%s", result);
        free(result);
    }

    write(client_socket, response, strlen(response));
    close(client_socket);
}

// Xóa bài kiểm tra
void handle_delete_exam(int client_socket, ControlMessage *msg)
{
    KeyValuePair pairs[10];
    int pair_count = parse_json(msg->body, pairs, 1);

    int exam_id = -1;
    for (int i = 0; i < pair_count; i++)
    {
        if (strcmp(pairs[i].key, "exam_id") == 0)
        {
            exam_id = atoi(pairs[i].value);
        }
    }

    int result = delete_exam(exam_id);
    char response[2048];
    char timestamp[50];
    time_t now = time(NULL);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%S", localtime(&now));

    if (result == 0)
    {
        snprintf(response, sizeof(response),
                 "NOTIFICATION DELETE_EXAM_FAILURE %s\n{\"message\": \"Failed to delete exam\"}",
                 timestamp);
    }
    else
    {
        snprintf(response, sizeof(response),
                 "NOTIFICATION DELETE_EXAM_SUCCESS %s\n{\"message\": \"Exam deleted successfully\"}",
                 timestamp);
    }

    write(client_socket, response, strlen(response));
    close(client_socket);
}

// Cập nhật trạng thái bài kiểm tra
void handle_update_exam_status(int client_socket, ControlMessage *msg)
{
    KeyValuePair pairs[10];
    int pair_count = parse_json(msg->body, pairs, 2);

    int exam_id = -1;
    char status[32] = "";

    for (int i = 0; i < pair_count; i++)
    {
        if (strcmp(pairs[i].key, "exam_id") == 0)
        {
            exam_id = atoi(pairs[i].value);
        }
        else if (strcmp(pairs[i].key, "status") == 0)
        {
            strncpy(status, pairs[i].value, sizeof(status) - 1);
        }
    }

    int result = update_exam_status(exam_id, status);
    char response[2048];
    char timestamp[50];
    time_t now = time(NULL);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%S", localtime(&now));

    if (result == 0)
    {
        snprintf(response, sizeof(response),
                 "NOTIFICATION UPDATE_EXAM_STATUS_FAILURE %s\n{\"message\": \"Failed to update exam status\"}",
                 timestamp);
    }
    else
    {
        snprintf(response, sizeof(response),
                 "NOTIFICATION UPDATE_EXAM_STATUS_SUCCESS %s\n{\"message\": \"Exam status updated successfully\"}",
                 timestamp);
    }

    write(client_socket, response, strlen(response));
    close(client_socket);
}

// Thêm câu hỏi vào bài kiểm tra
void handle_add_exam_question(int client_socket, ControlMessage *msg)
{
    KeyValuePair pairs[10];
    int pair_count = parse_json(msg->body, pairs, 10);

    int exam_id = -1, difficulty = 1;
    char content[1024] = "";
    cJSON *answer_list = NULL;

    for (int i = 0; i < pair_count; i++)
    {
        if (strcmp(pairs[i].key, "exam_id") == 0)
        {
            exam_id = atoi(pairs[i].value);
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
    int result = add_exam_question(exam_id, content, difficulty, &question_id);
    char response[2048];
    char timestamp[50];
    time_t now = time(NULL);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%S", localtime(&now));

    if (result == 0)
    {
        snprintf(response, sizeof(response),
                 "NOTIFICATION ADD_EXAM_QUESTION_FAILURE %s\n{\"message\": \"Failed to add question\"}",
                 timestamp);
    }
    else
    {
        // Thêm các đáp án
        if (answer_list != NULL)
        {
            cJSON *answer;
            cJSON_ArrayForEach(answer, answer_list)
            {
                cJSON *answer_content = cJSON_GetObjectItem(answer, "content");
                cJSON *is_correct = cJSON_GetObjectItem(answer, "is_correct");

                if (cJSON_IsString(answer_content) && cJSON_IsBool(is_correct))
                {
                    add_exam_answer(question_id, answer_content->valuestring, is_correct->valueint);
                }
            }
        }

        snprintf(response, sizeof(response),
                 "NOTIFICATION ADD_EXAM_QUESTION_SUCCESS %s\n{\"message\": \"Question added successfully\", \"question_id\": %d}",
                 timestamp, question_id);
    }

    write(client_socket, response, strlen(response));
    close(client_socket);

    if (answer_list != NULL)
    {
        cJSON_Delete(answer_list);
    }
}

// Lấy danh sách câu hỏi của bài kiểm tra
void handle_get_exam_questions(int client_socket, ControlMessage *msg)
{
    KeyValuePair pairs[10];
    int pair_count = parse_json(msg->body, pairs, 1);

    int exam_id = -1;
    for (int i = 0; i < pair_count; i++)
    {
        if (strcmp(pairs[i].key, "exam_id") == 0)
        {
            exam_id = atoi(pairs[i].value);
        }
    }

    char *result = get_exam_questions(exam_id);
    char *response;
    
    if (result == NULL)
    {
        response = strdup("DATA JSON EXAM_QUESTIONS\n{\"data\": []}");
    }
    else
    {
        size_t response_size = strlen(result) + 64;
        response = (char *)malloc(response_size);
        snprintf(response, response_size, "DATA JSON EXAM_QUESTIONS\n{\"data\": %s}", result);
        free(result);
    }

    write(client_socket, response, strlen(response));
    close(client_socket);
    free(response);
}

// Xóa câu hỏi
void handle_delete_exam_question(int client_socket, ControlMessage *msg)
{
    KeyValuePair pairs[10];
    int pair_count = parse_json(msg->body, pairs, 1);

    int question_id = -1;
    for (int i = 0; i < pair_count; i++)
    {
        if (strcmp(pairs[i].key, "question_id") == 0)
        {
            question_id = atoi(pairs[i].value);
        }
    }

    int result = delete_exam_question(question_id);
    char response[2048];
    char timestamp[50];
    time_t now = time(NULL);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%S", localtime(&now));

    if (result == 0)
    {
        snprintf(response, sizeof(response),
                 "NOTIFICATION DELETE_EXAM_QUESTION_FAILURE %s\n{\"message\": \"Failed to delete question\"}",
                 timestamp);
    }
    else
    {
        snprintf(response, sizeof(response),
                 "NOTIFICATION DELETE_EXAM_QUESTION_SUCCESS %s\n{\"message\": \"Question deleted successfully\"}",
                 timestamp);
    }

    write(client_socket, response, strlen(response));
    close(client_socket);
}

// Nộp bài kiểm tra
void handle_submit_exam_answers(int client_socket, ControlMessage *msg)
{
    KeyValuePair pairs[10];
    int pair_count = parse_json(msg->body, pairs, 10);

    int exam_id = -1, user_id = -1;
    char start_time[64] = "", end_time[64] = "";
    char *answers_json = NULL;

    for (int i = 0; i < pair_count; i++)
    {
        if (strcmp(pairs[i].key, "exam_id") == 0)
        {
            exam_id = atoi(pairs[i].value);
        }
        else if (strcmp(pairs[i].key, "user_id") == 0)
        {
            user_id = atoi(pairs[i].value);
        }
        else if (strcmp(pairs[i].key, "start_time") == 0)
        {
            strncpy(start_time, pairs[i].value, sizeof(start_time) - 1);
        }
        else if (strcmp(pairs[i].key, "end_time") == 0)
        {
            strncpy(end_time, pairs[i].value, sizeof(end_time) - 1);
        }
        else if (strcmp(pairs[i].key, "answers") == 0)
        {
            answers_json = pairs[i].value;
        }
    }

    int user_exam_id = submit_exam_answers(exam_id, user_id, start_time, end_time, answers_json);

    char response[2048];
    char timestamp[50];
    time_t now = time(NULL);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%S", localtime(&now));

    if (user_exam_id == 0)
    {
        snprintf(response, sizeof(response),
                 "NOTIFICATION SUBMIT_EXAM_FAILURE %s\n{\"message\": \"Failed to submit exam answers\"}",
                 timestamp);
    }
    else
    {
        snprintf(response, sizeof(response),
                 "NOTIFICATION SUBMIT_EXAM_SUCCESS %s\n{\"message\": \"Exam submitted successfully\", \"user_exam_id\": %d}",
                 timestamp, user_exam_id);
    }

    write(client_socket, response, strlen(response));
    close(client_socket);
}

// Lấy kết quả bài kiểm tra của sinh viên
void handle_get_exam_results(int client_socket, ControlMessage *msg)
{
    KeyValuePair pairs[10];
    int pair_count = parse_json(msg->body, pairs, 2);

    int exam_id = -1, user_id = -1;

    for (int i = 0; i < pair_count; i++)
    {
        if (strcmp(pairs[i].key, "exam_id") == 0)
        {
            exam_id = atoi(pairs[i].value);
        }
        else if (strcmp(pairs[i].key, "user_id") == 0)
        {
            user_id = atoi(pairs[i].value);
        }
    }

    char *result = get_exam_results(exam_id, user_id);
    char response[8192];
    memset(response, 0, sizeof(response));

    if (result == NULL)
    {
        snprintf(response, sizeof(response), "DATA JSON EXAM_RESULTS\n{\"message\": \"No results found\"}");
    }
    else
    {
        snprintf(response, sizeof(response), "DATA JSON EXAM_RESULTS\n%s", result);
        free(result);
    }

    write(client_socket, response, strlen(response));
    close(client_socket);
}

// Lấy danh sách điểm của tất cả sinh viên (teacher view)
void handle_get_exam_scores(int client_socket, ControlMessage *msg)
{
    KeyValuePair pairs[10];
    int pair_count = parse_json(msg->body, pairs, 1);

    int exam_id = -1;

    for (int i = 0; i < pair_count; i++)
    {
        if (strcmp(pairs[i].key, "exam_id") == 0)
        {
            exam_id = atoi(pairs[i].value);
        }
    }

    char *result = get_exam_scores(exam_id);
    char *response;

    if (result == NULL)
    {
        response = strdup("DATA JSON EXAM_SCORES\n{\"data\": []}");
    }
    else
    {
        size_t response_size = strlen(result) + 64;
        response = (char *)malloc(response_size);
        snprintf(response, response_size, "DATA JSON EXAM_SCORES\n{\"data\": %s}", result);
        free(result);
    }

    write(client_socket, response, strlen(response));
    close(client_socket);
    free(response);
}