#ifndef AUTH_SERVICE_H
#define AUTH_SERVICE_H

int signup(const char *email, const char *password, const char *username, const char *dob);
int login(const char *email, const char *password);

#endif // AUTH_SERVICE_H