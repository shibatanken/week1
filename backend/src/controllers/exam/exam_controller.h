#ifndef EXAM_CONTROLLER_H
#define EXAM_CONTROLLER_H

#include "../../data_structures/control_message.h"

// Exam CRUD Handlers
void handle_create_exam(int client_socket, ControlMessage *msg);
void handle_get_exams_in_class(int client_socket, ControlMessage *msg);
void handle_get_exam_detail(int client_socket, ControlMessage *msg);
void handle_delete_exam(int client_socket, ControlMessage *msg);
void handle_update_exam_status(int client_socket, ControlMessage *msg);

// Exam Question Handlers
void handle_add_exam_question(int client_socket, ControlMessage *msg);
void handle_get_exam_questions(int client_socket, ControlMessage *msg);
void handle_delete_exam_question(int client_socket, ControlMessage *msg);
void handle_add_question_to_exam(int client_socket, ControlMessage *msg);
void handle_add_question_to_bank(int client_socket, ControlMessage *msg);
void handle_get_question_bank(int client_socket, ControlMessage *msg);
void handle_import_question_from_bank(int client_socket, ControlMessage *msg);
void handle_delete_question_from_bank(int client_socket, ControlMessage *msg);

// Student Exam Flow Handlers
void handle_start_exam(int client_socket, ControlMessage *msg);
void handle_join_exam(int client_socket, ControlMessage *msg);
void handle_get_exam_for_student(int client_socket, ControlMessage *msg);
void handle_get_submission_status(int client_socket, ControlMessage *msg);
void handle_submit_answer(int client_socket, ControlMessage *msg);
void handle_submit_exam(int client_socket, ControlMessage *msg);
void handle_get_exam_result(int client_socket, ControlMessage *msg);
void handle_get_my_exam_history(int client_socket, ControlMessage *msg);

#endif // EXAM_CONTROLLER_H
