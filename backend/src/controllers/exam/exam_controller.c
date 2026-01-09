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
        if (strcmp(pairs[i].key, "exam_name") == 0) strncpy(exam_name, pairs[i].value, sizeof(exam_name) - 1);
        else if (strcmp(pairs[i].key, "description") == 0) strncpy(description, pairs[i].value, sizeof(description) - 1);
        else if (strcmp(pairs[i].key, "class_id") == 0) class_id = atoi(pairs[i].value);
        else if (strcmp(pairs[i].key, "time_limit") == 0) time_limit = atoi(pairs[i].value);
        else if (strcmp(pairs[i].key, "start_time") == 0) strncpy(start_time, pairs[i].value, sizeof(start_time) - 1);
        else if (strcmp(pairs[i].key, "end_time") == 0) strncpy(end_time, pairs[i].value, sizeof(end_time) - 1);
    }

    int exam_id = create_exam(exam_name, description, class_id, time_limit, start_time, end_time);
    char response[2048];
    char timestamp[50];
    time_t now = time(NULL);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%S", localtime(&now));

    if (exam_id == 0)
    {
        snprintf(response, sizeof(response), "NOTIFICATION CREATE_EXAM_FAILURE %s\n{\"message\": \"Failed to create exam\"}", timestamp);
    }
    else
    {
        snprintf(response, sizeof(response), "NOTIFICATION CREATE_EXAM_SUCCESS %s\n{\"message\": \"Exam created successfully\", \"exam_id\": %d}", timestamp, exam_id);
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
    for (int i = 0; i < pair_count; i++) { if (strcmp(pairs[i].key, "class_id") == 0) class_id = atoi(pairs[i].value); }

    char *result = get_exams_in_class(class_id);
    char response[16384]; // Increased size
    memset(response, 0, sizeof(response));

    if (result == NULL) snprintf(response, sizeof(response), "DATA JSON EXAMS_IN_CLASS\n{\"data\": []}");
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
    for (int i = 0; i < pair_count; i++) { if (strcmp(pairs[i].key, "exam_id") == 0) exam_id = atoi(pairs[i].value); }

    char *result = get_exam_detail(exam_id);
    char response[8192];
    memset(response, 0, sizeof(response));

    if (result == NULL) snprintf(response, sizeof(response), "DATA JSON EXAM_DETAIL\n{}");
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
    for (int i = 0; i < pair_count; i++) { if (strcmp(pairs[i].key, "exam_id") == 0) exam_id = atoi(pairs[i].value); }

    int result = delete_exam(exam_id);
    char response[2048];
    char timestamp[50];
    time_t now = time(NULL);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%S", localtime(&now));

    if (result == 0) snprintf(response, sizeof(response), "NOTIFICATION DELETE_EXAM_FAILURE %s\n{\"message\": \"Failed to delete exam\"}", timestamp);
    else snprintf(response, sizeof(response), "NOTIFICATION DELETE_EXAM_SUCCESS %s\n{\"message\": \"Exam deleted successfully\"}", timestamp);

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
        if (strcmp(pairs[i].key, "exam_id") == 0) exam_id = atoi(pairs[i].value);
        else if (strcmp(pairs[i].key, "status") == 0) strncpy(status, pairs[i].value, sizeof(status) - 1);
    }

    int result = update_exam_status(exam_id, status);
    char response[2048];
    char timestamp[50];
    time_t now = time(NULL);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%S", localtime(&now));

    if (result == 0) snprintf(response, sizeof(response), "NOTIFICATION UPDATE_EXAM_STATUS_FAILURE %s\n{\"message\": \"Failed to update exam status\"}", timestamp);
    else snprintf(response, sizeof(response), "NOTIFICATION UPDATE_EXAM_STATUS_SUCCESS %s\n{\"message\": \"Exam status updated successfully\"}", timestamp);

    write(client_socket, response, strlen(response));
    close(client_socket);
}

// --- NEW HANDLERS ---

// Thêm câu hỏi vào ngân hàng (Logic cũ của ADD_EXAM_QUESTION)
void handle_add_question_to_bank(int client_socket, ControlMessage *msg)
{
    KeyValuePair pairs[10];
    int pair_count = parse_json(msg->body, pairs, 10);

    int class_id = -1;
    char content[1024] = "", opt_a[256] = "", opt_b[256] = "", opt_c[256] = "", opt_d[256] = "", correct_option[5] = "A";

    for (int i = 0; i < pair_count; i++)
    {
        if (strcmp(pairs[i].key, "class_id") == 0) class_id = atoi(pairs[i].value);
        else if (strcmp(pairs[i].key, "content") == 0) strncpy(content, pairs[i].value, sizeof(content) - 1);
        else if (strcmp(pairs[i].key, "option_a") == 0) strncpy(opt_a, pairs[i].value, sizeof(opt_a) - 1);
        else if (strcmp(pairs[i].key, "option_b") == 0) strncpy(opt_b, pairs[i].value, sizeof(opt_b) - 1);
        else if (strcmp(pairs[i].key, "option_c") == 0) strncpy(opt_c, pairs[i].value, sizeof(opt_c) - 1);
        else if (strcmp(pairs[i].key, "option_d") == 0) strncpy(opt_d, pairs[i].value, sizeof(opt_d) - 1);
        else if (strcmp(pairs[i].key, "correct_option") == 0) strncpy(correct_option, pairs[i].value, sizeof(correct_option) - 1);
    }

    int result = add_question_to_class(class_id, content, opt_a, opt_b, opt_c, opt_d, correct_option);
    char response[2048];
    char timestamp[50];
    time_t now = time(NULL);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%S", localtime(&now));

    if (result == 0) snprintf(response, sizeof(response), "NOTIFICATION ADD_QUESTION_TO_BANK_FAILURE %s\n{\"message\": \"Failed to add question to bank\"}", timestamp);
    else snprintf(response, sizeof(response), "NOTIFICATION ADD_QUESTION_TO_BANK_SUCCESS %s\n{\"message\": \"Question added to bank successfully\"}", timestamp);

    write(client_socket, response, strlen(response));
    close(client_socket);
}

// Legacy handler mapped to Bank
void handle_add_exam_question(int client_socket, ControlMessage *msg)
{
   handle_add_question_to_bank(client_socket, msg);
}

// Thêm câu hỏi vào bài kiểm tra (Bảng exam_questions)
void handle_add_question_to_exam(int client_socket, ControlMessage *msg)
{
    KeyValuePair pairs[10];
    int pair_count = parse_json(msg->body, pairs, 10);

    int exam_id = -1;
    char content[1024] = "", opt_a[256] = "", opt_b[256] = "", opt_c[256] = "", opt_d[256] = "", correct_option[5] = "A";

    for (int i = 0; i < pair_count; i++)
    {
        if (strcmp(pairs[i].key, "exam_id") == 0) exam_id = atoi(pairs[i].value);
        else if (strcmp(pairs[i].key, "content") == 0) strncpy(content, pairs[i].value, sizeof(content) - 1);
        else if (strcmp(pairs[i].key, "option_a") == 0) strncpy(opt_a, pairs[i].value, sizeof(opt_a) - 1);
        else if (strcmp(pairs[i].key, "option_b") == 0) strncpy(opt_b, pairs[i].value, sizeof(opt_b) - 1);
        else if (strcmp(pairs[i].key, "option_c") == 0) strncpy(opt_c, pairs[i].value, sizeof(opt_c) - 1);
        else if (strcmp(pairs[i].key, "option_d") == 0) strncpy(opt_d, pairs[i].value, sizeof(opt_d) - 1);
        else if (strcmp(pairs[i].key, "correct_option") == 0) strncpy(correct_option, pairs[i].value, sizeof(correct_option) - 1);
    }

    int result = add_question_to_exam(exam_id, content, opt_a, opt_b, opt_c, opt_d, correct_option);
    char response[2048];
    char timestamp[50];
    time_t now = time(NULL);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%S", localtime(&now));

    if (result == 0) snprintf(response, sizeof(response), "NOTIFICATION ADD_QUESTION_TO_EXAM_FAILURE %s\n{\"message\": \"Failed to add question to exam\"}", timestamp);
    else snprintf(response, sizeof(response), "NOTIFICATION ADD_QUESTION_TO_EXAM_SUCCESS %s\n{\"message\": \"Question added to exam successfully\"}", timestamp);

    write(client_socket, response, strlen(response));
    close(client_socket);
}

// Import câu hỏi từ bank vào exam
void handle_import_question_from_bank(int client_socket, ControlMessage *msg)
{
    KeyValuePair pairs[10];
    int pair_count = parse_json(msg->body, pairs, 2);
    int exam_id = -1, question_id = -1;

    for (int i = 0; i < pair_count; i++)
    {
        if (strcmp(pairs[i].key, "exam_id") == 0) exam_id = atoi(pairs[i].value);
        else if (strcmp(pairs[i].key, "question_id") == 0) question_id = atoi(pairs[i].value);
    }

    int result = import_question_from_bank(exam_id, question_id);
    char response[2048];
    char timestamp[50];
    time_t now = time(NULL);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%S", localtime(&now));

    if (result == 0) snprintf(response, sizeof(response), "NOTIFICATION IMPORT_QUESTION_FAILURE %s\n{\"message\": \"Failed to import question\"}", timestamp);
    else snprintf(response, sizeof(response), "NOTIFICATION IMPORT_QUESTION_SUCCESS %s\n{\"message\": \"Question imported successfully\"}", timestamp);

    write(client_socket, response, strlen(response));
    close(client_socket);
}


// Lấy danh sách câu hỏi trong ngân hàng (theo class_id)
void handle_get_question_bank(int client_socket, ControlMessage *msg)
{
    KeyValuePair pairs[10];
    int pair_count = parse_json(msg->body, pairs, 1);
    int class_id = -1;
    for (int i = 0; i < pair_count; i++) { if (strcmp(pairs[i].key, "class_id") == 0) class_id = atoi(pairs[i].value); }

    char *result = get_questions_by_class(class_id);
    char *response;
    
    if (result == NULL) response = strdup("DATA JSON QUESTION_BANK\n{\"data\": []}");
    else
    {
        size_t response_size = strlen(result) + 64;
        response = (char *)malloc(response_size);
        snprintf(response, response_size, "DATA JSON QUESTION_BANK\n{\"data\": %s}", result);
        free(result);
    }

    write(client_socket, response, strlen(response));
    close(client_socket);
    free(response);
}

// Lấy danh sách câu hỏi CỦA BÀI THI (theo exam_id)
void handle_get_exam_questions(int client_socket, ControlMessage *msg)
{
    KeyValuePair pairs[10];
    int pair_count = parse_json(msg->body, pairs, 1);
    int exam_id = -1;
    for (int i = 0; i < pair_count; i++) { if (strcmp(pairs[i].key, "exam_id") == 0) exam_id = atoi(pairs[i].value); }

    char *result = get_exam_questions(exam_id);
    char *response;
    
    if (result == NULL) response = strdup("DATA JSON EXAM_QUESTIONS\n{\"data\": []}");
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

// Xóa câu hỏi trong bank
void handle_delete_question_from_bank(int client_socket, ControlMessage *msg)
{
    KeyValuePair pairs[10];
    int pair_count = parse_json(msg->body, pairs, 1);
    int question_id = -1;
    for (int i = 0; i < pair_count; i++) { if (strcmp(pairs[i].key, "question_id") == 0) question_id = atoi(pairs[i].value); }

    int result = delete_question_from_class(question_id);
    char response[2048];
    char timestamp[50];
    time_t now = time(NULL);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%S", localtime(&now));

    if (result == 0) snprintf(response, sizeof(response), "NOTIFICATION DELETE_QUESTION_FAILURE %s\n{\"message\": \"Failed to delete question\"}", timestamp);
    else snprintf(response, sizeof(response), "NOTIFICATION DELETE_QUESTION_SUCCESS %s\n{\"message\": \"Question deleted successfully\"}", timestamp);

    write(client_socket, response, strlen(response));
    close(client_socket);
}

// Legacy mapped to bank
void handle_delete_exam_question(int client_socket, ControlMessage *msg)
{
    handle_delete_question_from_bank(client_socket, msg);
}
