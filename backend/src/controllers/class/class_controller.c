#include "class_controller.h"
#include "../../data_structures/index.h"
#include "../../services/service.h"
#include "../../utils/json_utils.h"
#include "../../utils/log_utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

// Tạo lớp học mới (Teacher only)
void handle_create_class(int client_socket, ControlMessage *msg)
{
    KeyValuePair pairs[10];
    int pair_count = parse_json(msg->body, pairs, 10);

    char class_name[256] = "", description[512] = "";
    int teacher_id = -1;

    for (int i = 0; i < pair_count; i++)
    {
        if (strcmp(pairs[i].key, "class_name") == 0)
        {
            strncpy(class_name, pairs[i].value, sizeof(class_name) - 1);
        }
        else if (strcmp(pairs[i].key, "description") == 0)
        {
            strncpy(description, pairs[i].value, sizeof(description) - 1);
        }
        else if (strcmp(pairs[i].key, "teacher_id") == 0)
        {
            teacher_id = atoi(pairs[i].value);
        }
    }

    int result = create_class(class_name, description, teacher_id);
    char response[2048];
    char timestamp[50];
    time_t now = time(NULL);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%S", localtime(&now));

    if (result == 0)
    {
        snprintf(response, sizeof(response),
                 "NOTIFICATION CREATE_CLASS_FAILURE %s\n{\"message\": \"Failed to create class\"}",
                 timestamp);
    }
    else
    {
        snprintf(response, sizeof(response),
                 "NOTIFICATION CREATE_CLASS_SUCCESS %s\n{\"message\": \"Class created successfully\"}",
                 timestamp);
    }

    write(client_socket, response, strlen(response));
    close(client_socket);
}

// Lấy danh sách lớp học
void handle_get_class_list(int client_socket, ControlMessage *msg)
{
    char response[8192];
    memset(response, 0, sizeof(response));

    char *result = get_all_classes();
    if (result == NULL)
    {
        snprintf(response, sizeof(response), "DATA JSON GET_CLASS_LIST\n{\"data\": []}");
    }
    else
    {
        snprintf(response, sizeof(response), "DATA JSON GET_CLASS_LIST\n{\"data\": %s}", result);
        free(result);
    }

    write(client_socket, response, strlen(response));
    close(client_socket);
}

// Lấy danh sách lớp của user (phân biệt teacher/student)
void handle_get_my_classes(int client_socket, ControlMessage *msg)
{
    KeyValuePair pairs[10];
    int pair_count = parse_json(msg->body, pairs, 10);
    printf("role user: %s\n", msg->body);

    int user_id = -1;
    char role[20] = "";

    for (int i = 0; i < pair_count; i++) {
        if (strcmp(pairs[i].key, "user_id") == 0) {
            user_id = atoi(pairs[i].value);
        } else if (strcmp(pairs[i].key, "role") == 0) {
            strncpy(role, pairs[i].value, sizeof(role) - 1);
        }
    }

    char response[8192];
    memset(response, 0, sizeof(response));
    char *result = NULL;

    if (strcmp(role, "teacher") == 0) {
        result = get_classes_by_teacher(user_id);
    } else {
        result = get_classes_by_student(user_id);
    }

    if (result == NULL) {
        snprintf(response, sizeof(response), "DATA JSON GET_MY_CLASSES\n{\"data\": []}");
    } else {
        snprintf(response, sizeof(response), "DATA JSON GET_MY_CLASSES\n{\"data\": %s}", result);
        free(result);
    }

    write(client_socket, response, strlen(response));
    close(client_socket);
}
// Lấy chi tiết lớp học
void handle_get_class_detail(int client_socket, ControlMessage *msg)
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

    char response[8192];
    memset(response, 0, sizeof(response));

    char *result = get_class_detail(class_id);
    if (result == NULL)
    {
        snprintf(response, sizeof(response), "DATA JSON GET_CLASS_DETAIL\n{}");
    }
    else
    {
        snprintf(response, sizeof(response), "DATA JSON GET_CLASS_DETAIL\n%s", result);
        free(result);
    }

    write(client_socket, response, strlen(response));
    close(client_socket);
}

// Thêm sinh viên vào lớp (Teacher only)
void handle_add_student_to_class(int client_socket, ControlMessage *msg)
{
    KeyValuePair pairs[10];
    int pair_count = parse_json(msg->body, pairs, 2);

    int user_id = -1, class_id = -1;
    for (int i = 0; i < pair_count; i++)
    {
        if (strcmp(pairs[i].key, "user_id") == 0)
        {
            user_id = atoi(pairs[i].value);
        }
        else if (strcmp(pairs[i].key, "class_id") == 0)
        {
            class_id = atoi(pairs[i].value);
        }
    }

    int result = add_student_to_class(user_id, class_id);
    char response[2048];
    char timestamp[50];
    time_t now = time(NULL);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%S", localtime(&now));

    if (result == 0)
    {
        snprintf(response, sizeof(response),
                 "NOTIFICATION ADD_STUDENT_FAILURE %s\n{\"message\": \"Failed to add student\"}",
                 timestamp);
    }
    else
    {
        snprintf(response, sizeof(response),
                 "NOTIFICATION ADD_STUDENT_SUCCESS %s\n{\"message\": \"Student added successfully\"}",
                 timestamp);
    }

    write(client_socket, response, strlen(response));
    close(client_socket);
}

// Xóa sinh viên khỏi lớp (Teacher only)
void handle_remove_student_from_class(int client_socket, ControlMessage *msg)
{
    KeyValuePair pairs[10];
    int pair_count = parse_json(msg->body, pairs, 2);

    int user_id = -1, class_id = -1;
    for (int i = 0; i < pair_count; i++)
    {
        if (strcmp(pairs[i].key, "user_id") == 0)
        {
            user_id = atoi(pairs[i].value);
        }
        else if (strcmp(pairs[i].key, "class_id") == 0)
        {
            class_id = atoi(pairs[i].value);
        }
    }

    int result = remove_student_from_class(user_id, class_id);
    char response[2048];
    char timestamp[50];
    time_t now = time(NULL);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%S", localtime(&now));

    if (result == 0)
    {
        snprintf(response, sizeof(response),
                 "NOTIFICATION REMOVE_STUDENT_FAILURE %s\n{\"message\": \"Failed to remove student\"}",
                 timestamp);
    }
    else
    {
        snprintf(response, sizeof(response),
                 "NOTIFICATION REMOVE_STUDENT_SUCCESS %s\n{\"message\": \"Student removed successfully\"}",
                 timestamp);
    }

    write(client_socket, response, strlen(response));
    close(client_socket);
}

// Lấy danh sách sinh viên trong lớp
void handle_get_students_in_class(int client_socket, ControlMessage *msg)
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

    char response[8192];
    memset(response, 0, sizeof(response));

    char *result = get_students_in_class(class_id);
    if (result == NULL)
    {
        snprintf(response, sizeof(response), "DATA JSON STUDENTS_IN_CLASS\n{\"data\": []}");
    }
    else
    {
        snprintf(response, sizeof(response), "DATA JSON STUDENTS_IN_CLASS\n{\"data\": %s}", result);
        free(result);
    }

    write(client_socket, response, strlen(response));
    close(client_socket);
}

// Lấy danh sách sinh viên chưa có trong lớp
void handle_get_students_not_in_class(int client_socket, ControlMessage *msg)
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

    char response[8192];
    memset(response, 0, sizeof(response));

    char *result = get_students_not_in_class(class_id);
    if (result == NULL)
    {
        snprintf(response, sizeof(response), "DATA JSON STUDENTS_NOT_IN_CLASS\n{\"data\": []}");
    }
    else
    {
        snprintf(response, sizeof(response), "DATA JSON STUDENTS_NOT_IN_CLASS\n{\"data\": %s}", result);
        free(result);
    }

    write(client_socket, response, strlen(response));
    close(client_socket);
}




