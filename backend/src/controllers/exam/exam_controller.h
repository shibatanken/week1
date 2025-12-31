#ifndef EXAM_CONTROLLER_H
#define EXAM_CONTROLLER_H

#include "../../data_structures/index.h"

// Exam controllers
void handle_create_exam(int client_socket, ControlMessage *msg);
void handle_get_exams_in_class(int client_socket, ControlMessage *msg);
void handle_get_exam_detail(int client_socket, ControlMessage *msg);
void handle_delete_exam(int client_socket, ControlMessage *msg);
void handle_update_exam_status(int client_socket, ControlMessage *msg);

// Exam question controllers
void handle_add_exam_question(int client_socket, ControlMessage *msg);
void handle_get_exam_questions(int client_socket, ControlMessage *msg);
void handle_delete_exam_question(int client_socket, ControlMessage *msg);

// Exam scoring controllers
void handle_submit_exam_answers(int client_socket, ControlMessage *msg);
void handle_get_exam_results(int client_socket, ControlMessage *msg);
void handle_get_exam_scores(int client_socket, ControlMessage *msg);

#endif // EXAM_CONTROLLER_H