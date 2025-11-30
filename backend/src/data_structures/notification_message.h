#ifndef NOTIFICATION_MESSAGE_H
#define NOTIFICATION_MESSAGE_H

// Notification Types
#define LOGIN_SUCCESS "LOGIN_SUCCESS"
#define LOGIN_FAILURE "LOGIN_FAILURE"
#define ERROR_MESSAGE "ERROR_MESSAGE"

// Struct Definition
typedef struct
{
    char type[50];
    char timestamp[50];
    char body[1024];
} NotificationMessage;

#endif // NOTIFICATION_MESSAGE_H