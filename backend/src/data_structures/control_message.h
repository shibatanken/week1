#ifndef CONTROL_MESSAGE_H
#define CONTROL_MESSAGE_H

// Control Types - Authentication
#define LOGIN "LOGIN"
#define SIGN_UP "SIGN_UP"

// Control Types - Class Management
#define CREATE_CLASS "CREATE_CLASS"
#define GET_CLASS_LIST "GET_CLASS_LIST"
#define GET_CLASS_DETAIL "GET_CLASS_DETAIL"
#define ADD_STUDENT_TO_CLASS "ADD_STUDENT_TO_CLASS"
#define REMOVE_STUDENT_FROM_CLASS "REMOVE_STUDENT_FROM_CLASS"
#define GET_STUDENTS_IN_CLASS "GET_STUDENTS_IN_CLASS"
#define GET_STUDENTS_NOT_IN_CLASS "GET_STUDENTS_NOT_IN_CLASS"
#define DELETE_CLASS "DELETE_CLASS"

// Struct Definition
typedef struct
{
    char type[50];
    char body[1024];
} ControlMessage;

#endif // CONTROL_MESSAGE_H
