#ifndef CONTROL_MESSAGE_H
#define CONTROL_MESSAGE_H

// Control Types
#define LOGIN "LOGIN"
#define SIGN_UP "SIGN_UP"
#define CREATE_ROOM "CREATE_ROOM"
#define GET_ROOM_LIST "GET_ROOM_LIST"
#define ADD_QUESTION "ADD_QUESTION"
#define GET_ROOM_QUESTION "GET_ROOM_QUESTION"

// Struct Definition
typedef struct
{
    char type[50];
    char body[2048];
} ControlMessage;

#endif // CONTROL_MESSAGE_H