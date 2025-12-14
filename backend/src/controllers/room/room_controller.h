#ifndef ROOM_CONTROLLER_H
#define ROOM_CONTROLLER_H

#include "../../data_structures/control_message.h"

void handle_get_room_list(int client_socket, ControlMessage *msg);
void handle_create_room(int client_socket, ControlMessage *msg);
void handle_get_room_question(int client_socket, ControlMessage *msg);

#endif // ROOM_CONTROLLER_H