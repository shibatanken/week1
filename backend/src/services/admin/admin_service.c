#include "admin_service.h"
#include "../../db/connect-db.h"
#include "../../utils/time_utils.h"
#include "../../utils/log_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>

char *get_pending_users()
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL) return NULL;

    char query[512];
    snprintf(query, sizeof(query),
             "SELECT id, email, name, role, created_at FROM user WHERE status = 'pending' ORDER BY created_at DESC");

    if (mysql_query(conn, query)) return NULL;
    MYSQL_RES *res = mysql_store_result(conn);
    if (res == NULL) return NULL;

    cJSON *json_array = cJSON_CreateArray();
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        cJSON *obj = cJSON_CreateObject();
        cJSON_AddNumberToObject(obj, "id", atoi(row[0]));
        cJSON_AddStringToObject(obj, "email", row[1]);
        cJSON_AddStringToObject(obj, "name", row[2]);
        cJSON_AddStringToObject(obj, "role", row[3]);
        cJSON_AddStringToObject(obj, "created_at", row[4]);
        cJSON_AddItemToArray(json_array, obj);
    }

    mysql_free_result(res);
    char *json_string = cJSON_Print(json_array);
    cJSON_Delete(json_array);
    return json_string;
}

int approve_user(int user_id)
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL) return 0;

    char query[256];
    snprintf(query, sizeof(query), "UPDATE user SET status = 'approved' WHERE id = %d", user_id);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "Approve user failed. Error: %s\n", mysql_error(conn));
        return 0;
    }

    char *timestamp = get_current_time();
    char log_message[256];
    snprintf(log_message, sizeof(log_message), "User %d approved", user_id);
    log_to_file(log_message, timestamp);
    free(timestamp);

    return 1;
}

int reject_user(int user_id)
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL) return 0;

    char query[256];
    snprintf(query, sizeof(query), "UPDATE user SET status = 'rejected' WHERE id = %d", user_id);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "Reject user failed. Error: %s\n", mysql_error(conn));
        return 0;
    }

    char *timestamp = get_current_time();
    char log_message[256];
    snprintf(log_message, sizeof(log_message), "User %d rejected", user_id);
    log_to_file(log_message, timestamp);
    free(timestamp);

    return 1;
}

char *get_all_users()
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL) return NULL;

    char query[512];
    snprintf(query, sizeof(query),
             "SELECT id, email, name, role, status, created_at FROM user ORDER BY created_at DESC");

    if (mysql_query(conn, query)) return NULL;
    MYSQL_RES *res = mysql_store_result(conn);
    if (res == NULL) return NULL;

    cJSON *json_array = cJSON_CreateArray();
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        cJSON *obj = cJSON_CreateObject();
        cJSON_AddNumberToObject(obj, "id", atoi(row[0]));
        cJSON_AddStringToObject(obj, "email", row[1]);
        cJSON_AddStringToObject(obj, "name", row[2]);
        cJSON_AddStringToObject(obj, "role", row[3]);
        cJSON_AddStringToObject(obj, "status", row[4] ? row[4] : "approved");
        cJSON_AddStringToObject(obj, "created_at", row[5]);
        cJSON_AddItemToArray(json_array, obj);
    }

    mysql_free_result(res);
    char *json_string = cJSON_Print(json_array);
    cJSON_Delete(json_array);
    return json_string;
}

int admin_add_user(const char *email, const char *password, const char *name, const char *role)
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL) return 0;

    char query[1024];
    snprintf(query, sizeof(query),
             "INSERT INTO user (email, pass, name, role, status, created_at) "
             "VALUES ('%s', '%s', '%s', '%s', 'approved', NOW())",
             email, password, name, role);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "Admin add user failed. Error: %s\n", mysql_error(conn));
        return 0;
    }

    char *timestamp = get_current_time();
    char log_message[256];
    snprintf(log_message, sizeof(log_message), "Admin created user: %s", email);
    log_to_file(log_message, timestamp);
    free(timestamp);

    return (int)mysql_insert_id(conn);
}

int admin_update_user(int user_id, const char *email, const char *name, const char *role, const char *status)
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL) return 0;

    char query[1024];
    snprintf(query, sizeof(query),
             "UPDATE user SET email = '%s', name = '%s', role = '%s', status = '%s' WHERE id = %d",
             email, name, role, status, user_id);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "Admin update user failed. Error: %s\n", mysql_error(conn));
        return 0;
    }

    char *timestamp = get_current_time();
    char log_message[256];
    snprintf(log_message, sizeof(log_message), "Admin updated user %d", user_id);
    log_to_file(log_message, timestamp);
    free(timestamp);

    return 1;
}

int admin_delete_user(int user_id)
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL) return 0;

    char query[256];
    snprintf(query, sizeof(query), "DELETE FROM user WHERE id = %d", user_id);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "Admin delete user failed. Error: %s\n", mysql_error(conn));
        return 0;
    }

    char *timestamp = get_current_time();
    char log_message[256];
    snprintf(log_message, sizeof(log_message), "Admin deleted user %d", user_id);
    log_to_file(log_message, timestamp);
    free(timestamp);

    return 1;
}

