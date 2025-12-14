#include "question_service.h"
#include "../../db/connect-db.h"
#include "../../utils/mysql_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int add_question(int room_id, const char *content, int difficulty, int *question_id)
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL)
    {   
        fprintf(stderr, "Database connection failed.\n");
        return 0;
    }

    char query[1024];
    snprintf(query, sizeof(query),
             "INSERT INTO question (room_id, content, difficulty) VALUES (%d, '%s', %d)",
             room_id, content, difficulty);

    if (mysql_query(conn, query))
    {
        fprintf(stderr, "Add question failed. Error: %s\n", mysql_error(conn));
        return 0;
    }

    *question_id = mysql_insert_id(conn);
    return 1;
}

int add_answer(int question_id, const char *content, int is_true)
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL)
    {
        fprintf(stderr, "Database connection failed.\n");
        return 0;
    }

    char query[1024];
    snprintf(query, sizeof(query),
             "INSERT INTO answer_of_question (question_id, content, is_true) VALUES (%d, '%s', %d)",
             question_id, content, is_true);

    if (mysql_query(conn, query))
    {
        fprintf(stderr, "Add answer failed. Error: %s\n", mysql_error(conn));
        return 0;
    }

    return 1;
}
