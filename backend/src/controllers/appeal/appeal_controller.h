#ifndef APPEAL_CONTROLLER_H
#define APPEAL_CONTROLLER_H

#include "../../data_structures/control_message.h"

void handle_submit_appeal(int client_socket, ControlMessage *msg);
void handle_get_my_appeals(int client_socket, ControlMessage *msg);
void handle_get_appeals_for_teacher(int client_socket, ControlMessage *msg);
void handle_review_appeal(int client_socket, ControlMessage *msg);
void handle_get_unread_appeals_count(int client_socket, ControlMessage *msg);
void handle_mark_appeal_as_read(int client_socket, ControlMessage *msg);

#endif // APPEAL_CONTROLLER_H

