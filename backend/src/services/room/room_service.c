#include "room_service.h"
#include "../../db/connect-db.h"
#include "../../utils/mysql_utils.h"
#include "../../utils/time_utils.h"
#include "../../utils/log_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>

char *get_all_room()
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL)
    {
        fprintf(stderr, "Database connection failed.\n");
        return NULL;
    }

    const char *query = "SELECT id, subject, description, number_of_easy_question, number_of_medium_question, number_of_hard_question, time_limit, start, end FROM room";
    if (mysql_query(conn, query))
    {
        fprintf(stderr, "Query failed. Error: %s\n", mysql_error(conn));
        return NULL;
    }

    MYSQL_RES *res = mysql_store_result(conn);
    if (res == NULL)
    {
        fprintf(stderr, "mysql_store_result() failed. Error: %s\n", mysql_error(conn));
        return NULL;
    }

    int num_rows = mysql_num_rows(res);
    if (num_rows == 0)
    {
        mysql_free_result(res);
        return NULL;
    }

    MYSQL_ROW row;
    cJSON *json_array = cJSON_CreateArray();

    while ((row = mysql_fetch_row(res)))
    {
        cJSON *room = cJSON_CreateObject();
        cJSON_AddNumberToObject(room, "id", atoi(row[0]));
        cJSON_AddStringToObject(room, "subject", row[1] ? row[1] : "");
        cJSON_AddStringToObject(room, "description", row[2] ? row[2] : "");
        cJSON_AddNumberToObject(room, "number_of_easy_question", row[3] ? atoi(row[3]) : 0);
        cJSON_AddNumberToObject(room, "number_of_medium_question", row[4] ? atoi(row[4]) : 0);
        cJSON_AddNumberToObject(room, "number_of_hard_question", row[5] ? atoi(row[5]) : 0);
        cJSON_AddNumberToObject(room, "time_limit", row[6] ? atoi(row[6]) : 0);
        cJSON_AddStringToObject(room, "start", row[7] ? row[7] : "");
        cJSON_AddStringToObject(room, "end", row[8] ? row[8] : "");
        cJSON_AddItemToArray(json_array, room);
    }

    mysql_free_result(res);

    char *json_string = cJSON_Print(json_array);
    cJSON_Delete(json_array);

    return json_string;
}

int create_room(const char *subject, const char *description, int easy, int medium, int hard, int time_limit, const char *start, const char *end)
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL)
    {
        fprintf(stderr, "Database connection failed.\n");
        return 0;
    }

    // Convert ISO format (2025-12-13T22:16:22) to MySQL format (2025-12-13 22:16:22)
    char start_mysql[64], end_mysql[64];
    strncpy(start_mysql, start, sizeof(start_mysql) - 1);
    strncpy(end_mysql, end, sizeof(end_mysql) - 1);
    
    // Replace 'T' with space
    for (int i = 0; start_mysql[i]; i++) {
        if (start_mysql[i] == 'T') start_mysql[i] = ' ';
    }
    for (int i = 0; end_mysql[i]; i++) {
        if (end_mysql[i] == 'T') end_mysql[i] = ' ';
    }

    char query[1024];
    snprintf(query, sizeof(query),
             "INSERT INTO room (subject, description, number_of_easy_question, number_of_medium_question, number_of_hard_question, time_limit, start, end) VALUES ('%s', '%s', %d, %d, %d, %d, '%s', '%s')",
             subject, description, easy, medium, hard, time_limit, start_mysql, end_mysql);

    fprintf(stderr, "SQL Query: %s\n", query);

    if (mysql_query(conn, query))
    {
        fprintf(stderr, "Create room failed. Error: %s\n", mysql_error(conn));
        return 0;
    }

    // Log
    char *timestamp = get_current_time();
    char log_message[256];
    snprintf(log_message, sizeof(log_message), "Room created: %s", subject);
    log_to_file(log_message, timestamp);
    free(timestamp);

    return 1;
}

char *get_room_question(int room_id)
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL)
    {
        fprintf(stderr, "Database connection failed.\n");
        return NULL;
    }

    char query[256];
    snprintf(query, sizeof(query), "SELECT id, content, difficulty FROM question WHERE room_id = %d", room_id);
    if (mysql_query(conn, query))
    {
        fprintf(stderr, "Query failed. Error: %s\n", mysql_error(conn));
        return NULL;
    }

    MYSQL_RES *res = mysql_store_result(conn);
    if (res == NULL)
    {
        fprintf(stderr, "mysql_store_result() failed. Error: %s\n", mysql_error(conn));
        return NULL;
    }

    int num_rows = mysql_num_rows(res);
    if (num_rows == 0)
    {
        mysql_free_result(res);
        return NULL;
    }

    MYSQL_ROW row;
    cJSON *json_array = cJSON_CreateArray();

    while ((row = mysql_fetch_row(res)))
    {
        cJSON *question = cJSON_CreateObject();
        cJSON_AddNumberToObject(question, "id", atoi(row[0]));
        cJSON_AddStringToObject(question, "content", row[1] ? row[1] : "");
        cJSON_AddNumberToObject(question, "difficulty", row[2] ? atoi(row[2]) : 0);
        cJSON_AddItemToArray(json_array, question);
    }

    mysql_free_result(res);

    char *json_string = cJSON_Print(json_array);
    cJSON_Delete(json_array);

    return json_string;
}
