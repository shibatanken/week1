#ifndef AUTH_SERVICE_H
#define AUTH_SERVICE_H

typedef struct {
    int user_id;
    char role[20]; // "student" or "teacher"
} LoginResult;

int signup(const char *email, const char *password, const char *username, const char *dob);
LoginResult login(const char *email, const char *password);

#endif // AUTH_SERVICE_H