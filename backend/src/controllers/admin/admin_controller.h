#ifndef ADMIN_CONTROLLER_H
#define ADMIN_CONTROLLER_H

#include "../../data_structures/control_message.h"

void handle_admin_get_pending_users(int client_socket, ControlMessage *msg);
void handle_admin_approve_user(int client_socket, ControlMessage *msg);
void handle_admin_reject_user(int client_socket, ControlMessage *msg);
void handle_admin_get_all_users(int client_socket, ControlMessage *msg);
void handle_admin_add_user(int client_socket, ControlMessage *msg);
void handle_admin_update_user(int client_socket, ControlMessage *msg);
void handle_admin_delete_user(int client_socket, ControlMessage *msg);

#endif // ADMIN_CONTROLLER_H

