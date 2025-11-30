#ifndef AUTH_CONTROLLER_H
#define AUTH_CONTROLLER_H

#include "../../data_structures/index.h"

void handle_login(int client_socket, ControlMessage *msg);
void handle_signup(int client_socket, ControlMessage *msg);

#endif // AUTH_CONTROLLER_H