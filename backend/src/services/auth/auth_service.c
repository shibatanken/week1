#include "auth_service.h"
#include "../../db/connect-db.h"
#include "../../utils/mysql_utils.h"
#include "../../utils/time_utils.h"
#include "../../utils/log_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>
#include "../../data_structures/index.h"

int signup(const char *email, const char *password, const char *username, const char *dob)
{
    MYSQL *conn = get_db_connection();
    char query[512];
    // Default role là 'student', có thể thay đổi bằng cách thêm field role vào request
    snprintf(query, sizeof(query), "INSERT INTO user (email, pass, name, dob, role) VALUES ('%s', '%s', '%s', '%s', 'student')", email, password, username, dob);
    // printf("Query: %s\n", query);

    if (mysql_query(conn, query))
    {
        fprintf(stderr, "Signup failed. Error: %s\n", mysql_error(conn));
        return 0;
    }

    // Log the signup activity
    char *timestamp = get_current_time();
    snprintf(query, sizeof(query), "INSERT INTO log (log_content, log_time) VALUES ('User signed up with email: %s', '%s')", email, timestamp);
    if (mysql_query(conn, query))
    {
        fprintf(stderr, "Logging failed. Error: %s\n", mysql_error(conn));
    }

    // Log to file
    char log_message[256];
    snprintf(log_message, sizeof(log_message), "User signed up with email: %s", email);
    log_to_file(log_message, timestamp);

    free(timestamp);

    return 1;
}

LoginResult login(const char *email, const char *password)
{
    LoginResult result = {-1, ""};
    MYSQL *conn = get_db_connection();
    char query[512];
    snprintf(query, sizeof(query), "SELECT id, role FROM user WHERE email='%s' AND pass='%s'", email, password);

    if (mysql_query(conn, query))
    {
        fprintf(stderr, "Login failed. Error: %s\n", mysql_error(conn));
        return result;
    }

    MYSQL_RES *res = mysql_store_result(conn);
    if (res == NULL)
    {
        fprintf(stderr, "mysql_store_result() failed. Error: %s\n", mysql_error(conn));
        return result;
    }

    int num_rows = mysql_num_rows(res);
    if (num_rows > 0)
    {
        MYSQL_ROW row = mysql_fetch_row(res);
        result.user_id = atoi(row[0]);
        strncpy(result.role, row[1], sizeof(result.role) - 1);

        // Log the login activity
        char *timestamp = get_current_time();
        snprintf(query, sizeof(query), "INSERT INTO log (log_content, log_time) VALUES ('User logged in with email: %s (role: %s)', '%s')", email, result.role, timestamp);
        if (mysql_query(conn, query))
        {
            fprintf(stderr, "Logging failed. Error: %s\n", mysql_error(conn));
        }

        // Log to file
        char log_message[256];
        snprintf(log_message, sizeof(log_message), "User logged in with email: %s (role: %s)", email, result.role);
        log_to_file(log_message, timestamp);

        free(timestamp);
    }

    mysql_free_result(res);
    return result;
}
