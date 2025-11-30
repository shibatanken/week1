#ifndef CONTROL_MESSAGE_H
#define CONTROL_MESSAGE_H

// Control Types - Chỉ giữ LOGIN và SIGN_UP
#define LOGIN "LOGIN"
#define SIGN_UP "SIGN_UP"

// Struct Definition
typedef struct
{
    char type[50];
    char body[1024];
} ControlMessage;

#endif // CONTROL_MESSAGE_H
