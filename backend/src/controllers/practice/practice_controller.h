#ifndef PRACTICE_CONTROLLER_H
#define PRACTICE_CONTROLLER_H

#include "../../data_structures/control_message.h"

void handle_start_practice(int client_socket, ControlMessage *msg);
void handle_get_practice_questions(int client_socket, ControlMessage *msg);
void handle_submit_practice_answer(int client_socket, ControlMessage *msg);
void handle_finish_practice(int client_socket, ControlMessage *msg);
void handle_get_practice_history(int client_socket, ControlMessage *msg);

#endif // PRACTICE_CONTROLLER_H

