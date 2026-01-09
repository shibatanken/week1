#include "practice_service.h"
#include "../../db/connect-db.h"
#include "../../utils/time_utils.h"
#include "../../utils/log_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>

int start_practice(int user_id, int class_id, int num_questions)
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL) return 0;

    // Create practice session
    char query[512];
    snprintf(query, sizeof(query),
             "INSERT INTO practice_sessions (user_id, class_id, num_questions, status, started_at) "
             "VALUES (%d, %d, %d, 'in_progress', NOW())",
             user_id, class_id, num_questions);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "Start practice failed. Error: %s\n", mysql_error(conn));
        return 0;
    }

    int session_id = (int)mysql_insert_id(conn);

    // Randomly select questions from the class's question bank
    snprintf(query, sizeof(query),
             "INSERT INTO practice_answers (session_id, question_id) "
             "SELECT %d, id FROM questions WHERE class_id = %d ORDER BY RAND() LIMIT %d",
             session_id, class_id, num_questions);
    
    if (mysql_query(conn, query)) {
        fprintf(stderr, "Select practice questions failed. Error: %s\n", mysql_error(conn));
    }

    // Update total_questions count
    snprintf(query, sizeof(query),
             "UPDATE practice_sessions SET total_questions = "
             "(SELECT COUNT(*) FROM practice_answers WHERE session_id = %d) WHERE id = %d",
             session_id, session_id);
    mysql_query(conn, query);

    char *timestamp = get_current_time();
    char log_message[256];
    snprintf(log_message, sizeof(log_message), "Practice session %d started by user %d", session_id, user_id);
    log_to_file(log_message, timestamp);
    free(timestamp);

    return session_id;
}

char *get_practice_questions(int session_id)
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL) return NULL;

    char query[512];
    snprintf(query, sizeof(query),
             "SELECT q.id, q.content, q.option_a, q.option_b, q.option_c, q.option_d, pa.user_answer "
             "FROM practice_answers pa "
             "JOIN questions q ON pa.question_id = q.id "
             "WHERE pa.session_id = %d ORDER BY q.id", session_id);

    if (mysql_query(conn, query)) return NULL;
    MYSQL_RES *res = mysql_store_result(conn);
    if (res == NULL) return NULL;

    cJSON *json_array = cJSON_CreateArray();
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        cJSON *obj = cJSON_CreateObject();
        cJSON_AddNumberToObject(obj, "id", atoi(row[0]));
        cJSON_AddStringToObject(obj, "content", row[1]);
        cJSON_AddStringToObject(obj, "option_a", row[2]);
        cJSON_AddStringToObject(obj, "option_b", row[3]);
        cJSON_AddStringToObject(obj, "option_c", row[4]);
        cJSON_AddStringToObject(obj, "option_d", row[5]);
        cJSON_AddStringToObject(obj, "user_answer", row[6] ? row[6] : "");
        cJSON_AddItemToArray(json_array, obj);
    }

    mysql_free_result(res);
    char *json_string = cJSON_Print(json_array);
    cJSON_Delete(json_array);
    return json_string;
}

int submit_practice_answer(int session_id, int question_id, const char *answer)
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL) return 0;

    // Get correct answer
    char query[512];
    snprintf(query, sizeof(query),
             "SELECT correct_option FROM questions WHERE id = %d", question_id);

    if (mysql_query(conn, query)) return 0;
    MYSQL_RES *res = mysql_store_result(conn);
    if (res == NULL || mysql_num_rows(res) == 0) {
        mysql_free_result(res);
        return 0;
    }

    MYSQL_ROW row = mysql_fetch_row(res);
    int is_correct = (strcasecmp(row[0], answer) == 0) ? 1 : 0;
    mysql_free_result(res);

    // Update answer
    snprintf(query, sizeof(query),
             "UPDATE practice_answers SET user_answer = '%s', is_correct = %d, answered_at = NOW() "
             "WHERE session_id = %d AND question_id = %d",
             answer, is_correct, session_id, question_id);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "Submit practice answer failed. Error: %s\n", mysql_error(conn));
        return 0;
    }

    return 1;
}

int finish_practice(int session_id)
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL) return 0;

    char query[512];
    snprintf(query, sizeof(query),
             "UPDATE practice_sessions SET "
             "correct_answers = (SELECT COUNT(*) FROM practice_answers WHERE session_id = %d AND is_correct = 1), "
             "score = (SELECT COUNT(*) FROM practice_answers WHERE session_id = %d AND is_correct = 1) * 100.0 / "
             "NULLIF((SELECT COUNT(*) FROM practice_answers WHERE session_id = %d), 0), "
             "status = 'completed', completed_at = NOW() "
             "WHERE id = %d",
             session_id, session_id, session_id, session_id);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "Finish practice failed. Error: %s\n", mysql_error(conn));
        return 0;
    }

    return 1;
}

char *get_practice_result(int session_id)
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL) return NULL;

    char query[512];
    snprintf(query, sizeof(query),
             "SELECT ps.id, ps.score, ps.total_questions, ps.correct_answers, ps.status, "
             "ps.started_at, ps.completed_at, c.class_name "
             "FROM practice_sessions ps "
             "JOIN class c ON ps.class_id = c.id "
             "WHERE ps.id = %d", session_id);

    if (mysql_query(conn, query)) return NULL;
    MYSQL_RES *res = mysql_store_result(conn);
    if (res == NULL || mysql_num_rows(res) == 0) {
        mysql_free_result(res);
        return NULL;
    }

    MYSQL_ROW row = mysql_fetch_row(res);
    cJSON *obj = cJSON_CreateObject();
    cJSON_AddNumberToObject(obj, "session_id", atoi(row[0]));
    cJSON_AddNumberToObject(obj, "score", atof(row[1]));
    cJSON_AddNumberToObject(obj, "total_questions", atoi(row[2]));
    cJSON_AddNumberToObject(obj, "correct_answers", atoi(row[3]));
    cJSON_AddStringToObject(obj, "status", row[4]);
    cJSON_AddStringToObject(obj, "started_at", row[5] ? row[5] : "");
    cJSON_AddStringToObject(obj, "completed_at", row[6] ? row[6] : "");
    cJSON_AddStringToObject(obj, "class_name", row[7]);
    mysql_free_result(res);

    // Get detailed answers
    snprintf(query, sizeof(query),
             "SELECT pa.question_id, q.content, pa.user_answer, q.correct_option, pa.is_correct "
             "FROM practice_answers pa "
             "JOIN questions q ON pa.question_id = q.id "
             "WHERE pa.session_id = %d ORDER BY q.id", session_id);

    if (mysql_query(conn, query) == 0) {
        res = mysql_store_result(conn);
        if (res) {
            cJSON *answers_array = cJSON_CreateArray();
            while ((row = mysql_fetch_row(res))) {
                cJSON *answer_obj = cJSON_CreateObject();
                cJSON_AddNumberToObject(answer_obj, "question_id", atoi(row[0]));
                cJSON_AddStringToObject(answer_obj, "content", row[1]);
                cJSON_AddStringToObject(answer_obj, "user_answer", row[2] ? row[2] : "");
                cJSON_AddStringToObject(answer_obj, "correct_option", row[3]);
                cJSON_AddBoolToObject(answer_obj, "is_correct", atoi(row[4]));
                cJSON_AddItemToArray(answers_array, answer_obj);
            }
            cJSON_AddItemToObject(obj, "answers", answers_array);
            mysql_free_result(res);
        }
    }

    char *json_string = cJSON_Print(obj);
    cJSON_Delete(obj);
    return json_string;
}

char *get_practice_history(int user_id)
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL) return NULL;

    char query[512];
    snprintf(query, sizeof(query),
             "SELECT ps.id, c.class_name, ps.score, ps.total_questions, ps.correct_answers, "
             "ps.status, ps.started_at, ps.completed_at "
             "FROM practice_sessions ps "
             "JOIN class c ON ps.class_id = c.id "
             "WHERE ps.user_id = %d ORDER BY ps.started_at DESC", user_id);

    if (mysql_query(conn, query)) return NULL;
    MYSQL_RES *res = mysql_store_result(conn);
    if (res == NULL) return NULL;

    cJSON *json_array = cJSON_CreateArray();
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        cJSON *obj = cJSON_CreateObject();
        cJSON_AddNumberToObject(obj, "session_id", atoi(row[0]));
        cJSON_AddStringToObject(obj, "class_name", row[1]);
        cJSON_AddNumberToObject(obj, "score", atof(row[2]));
        cJSON_AddNumberToObject(obj, "total_questions", atoi(row[3]));
        cJSON_AddNumberToObject(obj, "correct_answers", atoi(row[4]));
        cJSON_AddStringToObject(obj, "status", row[5]);
        cJSON_AddStringToObject(obj, "started_at", row[6] ? row[6] : "");
        cJSON_AddStringToObject(obj, "completed_at", row[7] ? row[7] : "");
        cJSON_AddItemToArray(json_array, obj);
    }

    mysql_free_result(res);
    char *json_string = cJSON_Print(json_array);
    cJSON_Delete(json_array);
    return json_string;
}

