#ifndef CLASS_CONTROLLER_H
#define CLASS_CONTROLLER_H

#include "../../data_structures/index.h"

// Class management controllers
void handle_create_class(int client_socket, ControlMessage *msg);
void handle_get_class_list(int client_socket, ControlMessage *msg);
void handle_get_class_detail(int client_socket, ControlMessage *msg);
void handle_add_student_to_class(int client_socket, ControlMessage *msg);
void handle_remove_student_from_class(int client_socket, ControlMessage *msg);
void handle_get_students_in_class(int client_socket, ControlMessage *msg);
void handle_get_students_not_in_class(int client_socket, ControlMessage *msg);
void handle_get_my_classes(int client_socket, ControlMessage *msg);

#endif // CLASS_CONTROLLER_H


