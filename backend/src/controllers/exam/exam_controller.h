#ifndef EXAM_CONTROLLER_H
#define EXAM_CONTROLLER_H

#include "../../data_structures/control_message.h"

// Exam Handlers
void handle_create_exam(int client_socket, ControlMessage *msg);
void handle_get_exams_in_class(int client_socket, ControlMessage *msg);
void handle_get_exam_detail(int client_socket, ControlMessage *msg);
void handle_delete_exam(int client_socket, ControlMessage *msg);
void handle_update_exam_status(int client_socket, ControlMessage *msg);

// Exam Question Handlers (Legacy / Bank)
void handle_add_exam_question(int client_socket, ControlMessage *msg); // Creates in Bank
void handle_get_exam_questions(int client_socket, ControlMessage *msg); // Params: exam_id. Gets from Exam Questions table
void handle_delete_exam_question(int client_socket, ControlMessage *msg);

// New Handlers
void handle_add_question_to_exam(int client_socket, ControlMessage *msg);
void handle_add_question_to_bank(int client_socket, ControlMessage *msg);
void handle_get_question_bank(int client_socket, ControlMessage *msg);
void handle_import_question_from_bank(int client_socket, ControlMessage *msg);
void handle_delete_question_from_bank(int client_socket, ControlMessage *msg);


#endif // EXAM_CONTROLLER_H
