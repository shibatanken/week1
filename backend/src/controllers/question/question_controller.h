#ifndef QUESTION_CONTROLLER_H
#define QUESTION_CONTROLLER_H

#include "../../data_structures/control_message.h"

void handle_add_question(int client_socket, ControlMessage *msg);

#endif // QUESTION_CONTROLLER_H