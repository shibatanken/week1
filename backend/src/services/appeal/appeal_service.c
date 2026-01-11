#include "appeal_service.h"
#include "../../db/connect-db.h"
#include "../../utils/time_utils.h"
#include "../../utils/log_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>

int submit_appeal(int submission_id, int question_id, int user_id, const char *reason)
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL) return 0;

    char query[2048];
    char escaped_reason[1024];
    mysql_real_escape_string(conn, escaped_reason, reason, strlen(reason));
    
    snprintf(query, sizeof(query),
             "INSERT INTO appeals (submission_id, question_id, user_id, reason, status, created_at) "
             "VALUES (%d, %d, %d, '%s', 'pending', NOW())",
             submission_id, question_id, user_id, escaped_reason);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "Submit appeal failed. Error: %s\n", mysql_error(conn));
        return 0;
    }

    char *timestamp = get_current_time();
    char log_message[256];
    snprintf(log_message, sizeof(log_message), "Appeal submitted by user %d for question %d", user_id, question_id);
    log_to_file(log_message, timestamp);
    free(timestamp);

    return (int)mysql_insert_id(conn);
}

char *get_my_appeals(int user_id)
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL) return NULL;

    char query[1024];
    snprintf(query, sizeof(query),
             "SELECT a.id, a.submission_id, a.question_id, q.content as question_content, "
             "a.reason, a.status, a.teacher_response, a.teacher_comment, a.score_adjustment, "
             "a.created_at, a.resolved_at, e.exam_name, s.score as current_score "
             "FROM appeals a "
             "JOIN exam_questions q ON a.question_id = q.id "
             "JOIN exam_submissions s ON a.submission_id = s.id "
             "JOIN exam e ON s.exam_id = e.id "
             "WHERE a.user_id = %d ORDER BY a.created_at DESC", user_id);

    if (mysql_query(conn, query)) return NULL;
    MYSQL_RES *res = mysql_store_result(conn);
    if (res == NULL) return NULL;

    cJSON *json_array = cJSON_CreateArray();
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        cJSON *obj = cJSON_CreateObject();
        cJSON_AddNumberToObject(obj, "id", atoi(row[0]));
        cJSON_AddNumberToObject(obj, "submission_id", atoi(row[1]));
        cJSON_AddNumberToObject(obj, "question_id", atoi(row[2]));
        cJSON_AddStringToObject(obj, "question_content", row[3]);
        cJSON_AddStringToObject(obj, "reason", row[4]);
        cJSON_AddStringToObject(obj, "status", row[5]);
        cJSON_AddStringToObject(obj, "teacher_response", row[6] ? row[6] : "");
        cJSON_AddStringToObject(obj, "teacher_comment", row[7] ? row[7] : "");
        cJSON_AddNumberToObject(obj, "score_adjustment", row[8] ? atof(row[8]) : 0);
        cJSON_AddStringToObject(obj, "created_at", row[9]);
        cJSON_AddStringToObject(obj, "resolved_at", row[10] ? row[10] : "");
        cJSON_AddStringToObject(obj, "exam_name", row[11]);
        cJSON_AddNumberToObject(obj, "current_score", row[12] ? atof(row[12]) : 0.0);
        cJSON_AddItemToArray(json_array, obj);
    }

    mysql_free_result(res);
    char *json_string = cJSON_Print(json_array);
    cJSON_Delete(json_array);
    return json_string;
}

char *get_appeals_for_teacher(int teacher_id)
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL) return NULL;

    char query[1024];
    snprintf(query, sizeof(query),
             "SELECT a.id, a.submission_id, a.question_id, q.content as question_content, "
             "a.reason, a.status, a.teacher_response, a.teacher_comment, a.score_adjustment, "
             "a.created_at, a.resolved_at, e.exam_name, u.name as student_name, u.email, "
             "s.score as current_score "
             "FROM appeals a "
             "JOIN exam_questions q ON a.question_id = q.id "
             "JOIN exam_submissions s ON a.submission_id = s.id "
             "JOIN exam e ON s.exam_id = e.id "
             "JOIN class c ON e.class_id = c.id "
             "JOIN user u ON a.user_id = u.id "
             "WHERE c.teacher_id = %d ORDER BY a.created_at DESC", teacher_id);

    if (mysql_query(conn, query)) return NULL;
    MYSQL_RES *res = mysql_store_result(conn);
    if (res == NULL) return NULL;

    cJSON *json_array = cJSON_CreateArray();
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        cJSON *obj = cJSON_CreateObject();
        cJSON_AddNumberToObject(obj, "id", atoi(row[0]));
        cJSON_AddNumberToObject(obj, "submission_id", atoi(row[1]));
        cJSON_AddNumberToObject(obj, "question_id", atoi(row[2]));
        cJSON_AddStringToObject(obj, "question_content", row[3]);
        cJSON_AddStringToObject(obj, "reason", row[4]);
        cJSON_AddStringToObject(obj, "status", row[5]);
        cJSON_AddStringToObject(obj, "teacher_response", row[6] ? row[6] : "");
        cJSON_AddStringToObject(obj, "teacher_comment", row[7] ? row[7] : "");
        cJSON_AddNumberToObject(obj, "score_adjustment", row[8] ? atof(row[8]) : 0);
        cJSON_AddStringToObject(obj, "created_at", row[9]);
        cJSON_AddStringToObject(obj, "resolved_at", row[10] ? row[10] : "");
        cJSON_AddStringToObject(obj, "exam_name", row[11]);
        cJSON_AddStringToObject(obj, "student_name", row[12]);
        cJSON_AddStringToObject(obj, "student_email", row[13]);
        cJSON_AddNumberToObject(obj, "current_score", row[14] ? atof(row[14]) : 0.0);
        cJSON_AddItemToArray(json_array, obj);
    }

    mysql_free_result(res);
    char *json_string = cJSON_Print(json_array);
    cJSON_Delete(json_array);
    return json_string;
}

int review_appeal(int appeal_id, const char *status, const char *response, double score_adj, const char *teacher_comment)
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL) return 0;

    char query[2048];
    char escaped_response[1024] = "";
    char escaped_comment[1024] = "";
    
    if (response && strlen(response) > 0) {
        mysql_real_escape_string(conn, escaped_response, response, strlen(response));
    }
    if (teacher_comment && strlen(teacher_comment) > 0) {
        mysql_real_escape_string(conn, escaped_comment, teacher_comment, strlen(teacher_comment));
    }

    if (strlen(escaped_comment) > 0) {
        snprintf(query, sizeof(query),
                 "UPDATE appeals SET status = '%s', teacher_response = '%s', teacher_comment = '%s', "
                 "score_adjustment = %.2f, resolved_at = NOW(), teacher_read = TRUE, student_read = FALSE WHERE id = %d",
                 status, escaped_response, escaped_comment, score_adj, appeal_id);
    } else {
        snprintf(query, sizeof(query),
                 "UPDATE appeals SET status = '%s', teacher_response = '%s', "
                 "score_adjustment = %.2f, resolved_at = NOW(), teacher_read = TRUE, student_read = FALSE WHERE id = %d",
                 status, escaped_response, score_adj, appeal_id);
    }

    if (mysql_query(conn, query)) {
        fprintf(stderr, "Review appeal failed. Error: %s\n", mysql_error(conn));
        return 0;
    }

    // If approved with score adjustment, update the submission score
    if (strcmp(status, "approved") == 0 && score_adj != 0) {
        snprintf(query, sizeof(query),
                 "UPDATE exam_submissions s "
                 "JOIN appeals a ON a.submission_id = s.id "
                 "SET s.score = s.score + %.2f "
                 "WHERE a.id = %d", score_adj, appeal_id);
        mysql_query(conn, query);
    }

    char *timestamp = get_current_time();
    char log_message[256];
    snprintf(log_message, sizeof(log_message), "Appeal %d reviewed: %s", appeal_id, status);
    log_to_file(log_message, timestamp);
    free(timestamp);

    return 1;
}

// Get unread appeals count for user
int get_unread_appeals_count(int user_id, int is_teacher)
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL) return 0;

    char query[512];
    
    if (is_teacher) {
        // For teacher: count appeals where teacher_read = FALSE
        snprintf(query, sizeof(query),
                 "SELECT COUNT(*) FROM appeals a "
                 "JOIN exam_submissions s ON a.submission_id = s.id "
                 "JOIN exam e ON s.exam_id = e.id "
                 "JOIN class c ON e.class_id = c.id "
                 "WHERE c.teacher_id = %d AND a.teacher_read = FALSE", user_id);
    } else {
        // For student: count appeals where student_read = FALSE and status != 'pending'
        snprintf(query, sizeof(query),
                 "SELECT COUNT(*) FROM appeals "
                 "WHERE user_id = %d AND student_read = FALSE AND status != 'pending'", user_id);
    }

    if (mysql_query(conn, query)) return 0;
    
    MYSQL_RES *res = mysql_store_result(conn);
    if (res == NULL) return 0;
    
    MYSQL_ROW row = mysql_fetch_row(res);
    int count = row ? atoi(row[0]) : 0;
    
    mysql_free_result(res);
    return count;
}

// Mark appeal as read
int mark_appeal_as_read(int appeal_id, int user_id, int is_teacher)
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL) return 0;

    char query[512];
    
    if (is_teacher) {
        snprintf(query, sizeof(query),
                 "UPDATE appeals a "
                 "JOIN exam_submissions s ON a.submission_id = s.id "
                 "JOIN exam e ON s.exam_id = e.id "
                 "JOIN class c ON e.class_id = c.id "
                 "SET a.teacher_read = TRUE "
                 "WHERE a.id = %d AND c.teacher_id = %d", appeal_id, user_id);
    } else {
        snprintf(query, sizeof(query),
                 "UPDATE appeals SET student_read = TRUE "
                 "WHERE id = %d AND user_id = %d", appeal_id, user_id);
    }

    if (mysql_query(conn, query)) {
        fprintf(stderr, "Mark appeal as read failed. Error: %s\n", mysql_error(conn));
        return 0;
    }

    return 1;
}

