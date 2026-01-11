#include "../data_structures/index.h"
#include "../controllers/auth/auth_controller.h"
#include "../controllers/class/class_controller.h"
#include "../controllers/exam/exam_controller.h"
#include "../controllers/controller.h"
#include "routes.h"
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>

pthread_mutex_t lock;

#define PORT 8081
#define BACKLOG 10

typedef struct pthread_arg_t
{
    int new_socket_fd;
    struct sockaddr_in client_address;
} pthread_arg_t;

void *pthread_routine(void *arg);

void handle_control_message(int socket, ControlMessage *msg)
{

    if (strcmp(msg->type, LOGIN) == 0) handle_login(socket, msg);
    else if (strcmp(msg->type, SIGN_UP) == 0) handle_signup(socket, msg);
    else if (strcmp(msg->type, CREATE_CLASS) == 0) handle_create_class(socket, msg);
    else if (strcmp(msg->type, GET_CLASS_LIST) == 0) handle_get_class_list(socket, msg);
    else if (strcmp(msg->type, GET_CLASS_DETAIL) == 0) handle_get_class_detail(socket, msg);
    else if (strcmp(msg->type, ADD_STUDENT_TO_CLASS) == 0) handle_add_student_to_class(socket, msg);
    else if (strcmp(msg->type, REMOVE_STUDENT_FROM_CLASS) == 0) handle_remove_student_from_class(socket, msg);
    else if (strcmp(msg->type, GET_STUDENTS_IN_CLASS) == 0) handle_get_students_in_class(socket, msg);
    else if (strcmp(msg->type, GET_STUDENTS_NOT_IN_CLASS) == 0) handle_get_students_not_in_class(socket, msg);
    else if (strcmp(msg->type, GET_MY_CLASSES) == 0) handle_get_my_classes(socket, msg);
    else if (strcmp(msg->type, CREATE_EXAM) == 0) handle_create_exam(socket, msg);
    else if (strcmp(msg->type, GET_EXAMS_IN_CLASS) == 0) handle_get_exams_in_class(socket, msg);
    else if (strcmp(msg->type, GET_EXAM_DETAIL) == 0) handle_get_exam_detail(socket, msg);
    else if (strcmp(msg->type, DELETE_EXAM) == 0) handle_delete_exam(socket, msg);
    else if (strcmp(msg->type, UPDATE_EXAM_STATUS) == 0) handle_update_exam_status(socket, msg);
    
    // Exam Question Management
    else if (strcmp(msg->type, ADD_EXAM_QUESTION) == 0) handle_add_exam_question(socket, msg);
    else if (strcmp(msg->type, GET_EXAM_QUESTIONS) == 0) handle_get_exam_questions(socket, msg);
    else if (strcmp(msg->type, DELETE_EXAM_QUESTION) == 0) handle_delete_exam_question(socket, msg);
    
    // Question Bank Routes
    else if (strcmp(msg->type, ADD_QUESTION_TO_EXAM) == 0) handle_add_question_to_exam(socket, msg);
    else if (strcmp(msg->type, ADD_QUESTION_TO_BANK) == 0) handle_add_question_to_bank(socket, msg);
    else if (strcmp(msg->type, IMPORT_QUESTION_FROM_BANK) == 0) handle_import_question_from_bank(socket, msg);
    else if (strcmp(msg->type, GET_QUESTION_BANK) == 0) handle_get_question_bank(socket, msg);
    else if (strcmp(msg->type, DELETE_QUESTION_FROM_BANK) == 0) handle_delete_question_from_bank(socket, msg);
    
    // Student Exam Flow Routes
    else if (strcmp(msg->type, START_EXAM) == 0) handle_start_exam(socket, msg);
    else if (strcmp(msg->type, JOIN_EXAM) == 0) handle_join_exam(socket, msg);
    else if (strcmp(msg->type, GET_EXAM_FOR_STUDENT) == 0) handle_get_exam_for_student(socket, msg);
    else if (strcmp(msg->type, SUBMIT_ANSWER) == 0) handle_submit_answer(socket, msg);
    else if (strcmp(msg->type, SUBMIT_EXAM) == 0) handle_submit_exam(socket, msg);
    else if (strcmp(msg->type, GET_EXAM_RESULT) == 0) handle_get_exam_result(socket, msg);
    else if (strcmp(msg->type, GET_MY_EXAM_HISTORY) == 0) handle_get_my_exam_history(socket, msg);
    
    // Practice Mode Routes
    else if (strcmp(msg->type, START_PRACTICE) == 0) handle_start_practice(socket, msg);
    else if (strcmp(msg->type, GET_PRACTICE_QUESTIONS) == 0) handle_get_practice_questions(socket, msg);
    else if (strcmp(msg->type, SUBMIT_PRACTICE_ANSWER) == 0) handle_submit_practice_answer(socket, msg);
    else if (strcmp(msg->type, FINISH_PRACTICE) == 0) handle_finish_practice(socket, msg);
    else if (strcmp(msg->type, GET_PRACTICE_HISTORY) == 0) handle_get_practice_history(socket, msg);
    
    // Appeal Routes
    else if (strcmp(msg->type, SUBMIT_APPEAL) == 0) handle_submit_appeal(socket, msg);
    else if (strcmp(msg->type, GET_MY_APPEALS) == 0) handle_get_my_appeals(socket, msg);
    else if (strcmp(msg->type, GET_APPEALS_FOR_TEACHER) == 0) handle_get_appeals_for_teacher(socket, msg);
    else if (strcmp(msg->type, REVIEW_APPEAL) == 0) handle_review_appeal(socket, msg);
    else if (strcmp(msg->type, GET_UNREAD_APPEALS_COUNT) == 0) handle_get_unread_appeals_count(socket, msg);
    else if (strcmp(msg->type, MARK_APPEAL_AS_READ) == 0) handle_mark_appeal_as_read(socket, msg);
    
    // Admin Routes
    else if (strcmp(msg->type, ADMIN_GET_PENDING_USERS) == 0) handle_admin_get_pending_users(socket, msg);
    else if (strcmp(msg->type, ADMIN_APPROVE_USER) == 0) handle_admin_approve_user(socket, msg);
    else if (strcmp(msg->type, ADMIN_REJECT_USER) == 0) handle_admin_reject_user(socket, msg);
    else if (strcmp(msg->type, ADMIN_GET_ALL_USERS) == 0) handle_admin_get_all_users(socket, msg);
    else if (strcmp(msg->type, ADMIN_ADD_USER) == 0) handle_admin_add_user(socket, msg);
    else if (strcmp(msg->type, ADMIN_UPDATE_USER) == 0) handle_admin_update_user(socket, msg);
    else if (strcmp(msg->type, ADMIN_DELETE_USER) == 0) handle_admin_delete_user(socket, msg);
    
    // Statistics Routes
    else if (strcmp(msg->type, GET_EXAM_STATISTICS) == 0) handle_get_exam_statistics(socket, msg);
    else if (strcmp(msg->type, GET_CLASS_STATISTICS) == 0) handle_get_class_statistics(socket, msg);
    
    else printf("Unknown message type: '%s'\n", msg->type);
}

void handle_data_message(int socket, DataMessage *msg)
{
    // Placeholder
}

void handle_notification_message(int socket, NotificationMessage *msg)
{
    // Placeholder
}

void *pthread_routine(void *arg)
{
    pthread_arg_t *pthread_arg = (pthread_arg_t *)arg;
    int new_socket_fd = pthread_arg->new_socket_fd;

    char buffer[4096];
    memset(buffer, 0, sizeof(buffer));
    read(new_socket_fd, buffer, sizeof(buffer));
    printf("Received message: %s\n", buffer);

    char *header = strtok(buffer, "\n");
    char *body = header + strlen(header) + 1;

    printf("header: %s - body: %s\n", header, body);
    pthread_mutex_lock(&lock);
    if (strncmp(header, "CONTROL", 7) == 0)
    {
        ControlMessage msg;
        sscanf(header, "CONTROL %s", msg.type);
        strncpy(msg.body, body, sizeof(msg.body) - 1);
        handle_control_message(new_socket_fd, &msg);
    }
    else if (strncmp(header, "DATA", 4) == 0)
    {
        DataMessage msg;
        sscanf(header, "DATA %s %s %d", msg.type, msg.data_type, &msg.data_size);
        strncpy(msg.body, body, sizeof(msg.body) - 1);
        handle_data_message(new_socket_fd, &msg);
    }
    else if (strncmp(header, "NOTIFICATION", 12) == 0)
    {
        NotificationMessage msg;
        sscanf(header, "NOTIFICATION %s %s", msg.type, msg.timestamp);
        strncpy(msg.body, body, sizeof(msg.body) - 1);
        handle_notification_message(new_socket_fd, &msg);
    }
    pthread_mutex_unlock(&lock);

    close(new_socket_fd);
    free(pthread_arg);
    return NULL;
}

void setup_routes(int socket_fd)
{
    struct sockaddr_in address, client_address;
    socklen_t client_address_len;
    pthread_t thread_id;
    int new_socket_fd;

    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("Mutex init failed\n");
        return;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(socket_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(socket_fd, BACKLOG) < 0)
    {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    while (1)
    {
        client_address_len = sizeof(client_address);
        if ((new_socket_fd = accept(socket_fd, (struct sockaddr *)&client_address, &client_address_len)) < 0)
        {
            perror("Accept failed");
            continue;
        }

        pthread_arg_t *pthread_arg = (pthread_arg_t *)malloc(sizeof(pthread_arg_t));
        pthread_arg->new_socket_fd = new_socket_fd;
        pthread_arg->client_address = client_address;

        if (pthread_create(&thread_id, NULL, pthread_routine, (void *)pthread_arg) != 0)
        {
            perror("Thread create failed");
            free(pthread_arg);
        }
        pthread_detach(thread_id);
    }
}
