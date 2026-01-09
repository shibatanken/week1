#ifndef STATISTICS_CONTROLLER_H
#define STATISTICS_CONTROLLER_H

#include "../../data_structures/control_message.h"

void handle_get_exam_statistics(int client_socket, ControlMessage *msg);
void handle_get_class_statistics(int client_socket, ControlMessage *msg);

#endif // STATISTICS_CONTROLLER_H

