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
    char query[256];
    snprintf(query, sizeof(query), "INSERT INTO user (email, pass, name, dob) VALUES ('%s', '%s', '%s', '%s')", email, password, username, dob);
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

int login(const char *email, const char *password)
{
    int user_id;
    MYSQL *conn = get_db_connection();
    char query[256];
    snprintf(query, sizeof(query), "SELECT * FROM user WHERE email='%s' AND pass='%s'", email, password);
    // printf("Query: %s\n", query);

    if (mysql_query(conn, query))
    {
        fprintf(stderr, "Login failed. Error: %s\n", mysql_error(conn));
        return 0;
    }

    MYSQL_RES *res = mysql_store_result(conn);
    if (res == NULL)
    {
        fprintf(stderr, "mysql_store_result() failed. Error: %s\n", mysql_error(conn));
        return 0;
    }

    int num_rows = mysql_num_rows(res);
    if (num_rows > 0)
    {
        MYSQL_ROW row = mysql_fetch_row(res);
        user_id = atoi(row[0]); // Assuming the first column is the user id

        // Log the login activity
        char *timestamp = get_current_time();
        snprintf(query, sizeof(query), "INSERT INTO log (log_content, log_time) VALUES ('User logged in with email: %s', '%s')", email, timestamp);
        if (mysql_query(conn, query))
        {
            fprintf(stderr, "Logging failed. Error: %s\n", mysql_error(conn));
        }

        // Log to file
        char log_message[256];
        snprintf(log_message, sizeof(log_message), "User logged in with email: %s", email);
        log_to_file(log_message, timestamp);

        free(timestamp);
    }
    else
    {
        user_id = -1;
    }

    // print_mysql_result(res);
    mysql_free_result(res);

    return user_id;
}
