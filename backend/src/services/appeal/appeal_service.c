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

    char query[2048];
    snprintf(query, sizeof(query),
             "SELECT a.id, a.submission_id, a.question_id, q.content as question_content, "
             "q.option_a, q.option_b, q.option_c, q.option_d, q.correct_option, "
             "IFNULL(ea.user_answer, '') as student_answer, "
             "a.reason, a.status, a.teacher_response, a.teacher_comment, a.score_adjustment, "
             "a.created_at, a.resolved_at, e.exam_name, s.score as current_score "
             "FROM appeals a "
             "JOIN exam_questions q ON a.question_id = q.id "
             "JOIN exam_submissions s ON a.submission_id = s.id "
             "JOIN exam e ON s.exam_id = e.id "
             "LEFT JOIN exam_answers ea ON ea.submission_id = a.submission_id AND ea.question_id = a.question_id "
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
        cJSON_AddStringToObject(obj, "option_a", row[4]);
        cJSON_AddStringToObject(obj, "option_b", row[5]);
        cJSON_AddStringToObject(obj, "option_c", row[6]);
        cJSON_AddStringToObject(obj, "option_d", row[7]);
        cJSON_AddStringToObject(obj, "correct_option", row[8]);
        cJSON_AddStringToObject(obj, "student_answer", row[9]);
        cJSON_AddStringToObject(obj, "reason", row[10]);
        cJSON_AddStringToObject(obj, "status", row[11]);
        cJSON_AddStringToObject(obj, "teacher_response", row[12] ? row[12] : "");
        cJSON_AddStringToObject(obj, "teacher_comment", row[13] ? row[13] : "");
        cJSON_AddNumberToObject(obj, "score_adjustment", row[14] ? atof(row[14]) : 0);
        cJSON_AddStringToObject(obj, "created_at", row[15]);
        cJSON_AddStringToObject(obj, "resolved_at", row[16] ? row[16] : "");
        cJSON_AddStringToObject(obj, "exam_name", row[17]);
        cJSON_AddNumberToObject(obj, "current_score", row[18] ? atof(row[18]) : 0.0);
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

    char query[2048];
    snprintf(query, sizeof(query),
             "SELECT a.id, a.submission_id, a.question_id, q.content as question_content, "
             "q.option_a, q.option_b, q.option_c, q.option_d, q.correct_option, "
             "IFNULL(ea.user_answer, '') as student_answer, "
             "a.reason, a.status, a.teacher_response, a.teacher_comment, a.score_adjustment, "
             "a.created_at, a.resolved_at, e.exam_name, u.name as student_name, u.email, "
             "s.score as current_score "
             "FROM appeals a "
             "JOIN exam_questions q ON a.question_id = q.id "
             "JOIN exam_submissions s ON a.submission_id = s.id "
             "JOIN exam e ON s.exam_id = e.id "
             "JOIN class c ON e.class_id = c.id "
             "JOIN user u ON a.user_id = u.id "
             "LEFT JOIN exam_answers ea ON ea.submission_id = a.submission_id AND ea.question_id = a.question_id "
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
        cJSON_AddStringToObject(obj, "option_a", row[4]);
        cJSON_AddStringToObject(obj, "option_b", row[5]);
        cJSON_AddStringToObject(obj, "option_c", row[6]);
        cJSON_AddStringToObject(obj, "option_d", row[7]);
        cJSON_AddStringToObject(obj, "correct_option", row[8]);
        cJSON_AddStringToObject(obj, "student_answer", row[9]);
        cJSON_AddStringToObject(obj, "reason", row[10]);
        cJSON_AddStringToObject(obj, "status", row[11]);
        cJSON_AddStringToObject(obj, "teacher_response", row[12] ? row[12] : "");
        cJSON_AddStringToObject(obj, "teacher_comment", row[13] ? row[13] : "");
        cJSON_AddNumberToObject(obj, "score_adjustment", row[14] ? atof(row[14]) : 0);
        cJSON_AddStringToObject(obj, "created_at", row[15]);
        cJSON_AddStringToObject(obj, "resolved_at", row[16] ? row[16] : "");
        cJSON_AddStringToObject(obj, "exam_name", row[17]);
        cJSON_AddStringToObject(obj, "student_name", row[18]);
        cJSON_AddStringToObject(obj, "student_email", row[19]);
        cJSON_AddNumberToObject(obj, "current_score", row[20] ? atof(row[20]) : 0.0);
        cJSON_AddItemToArray(json_array, obj);
    }

    mysql_free_result(res);
    char *json_string = cJSON_Print(json_array);
    cJSON_Delete(json_array);
    return json_string;
}

int review_appeal(int appeal_id, int teacher_id, const char *status, const char *response, const char *teacher_comment)
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

    // Step 1: Get appeal details (submission_id, question_id, current score)
    snprintf(query, sizeof(query),
             "SELECT a.submission_id, a.question_id, s.score, s.exam_id "
             "FROM appeals a "
             "JOIN exam_submissions s ON a.submission_id = s.id "
             "WHERE a.id = %d", appeal_id);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "Get appeal details failed. Error: %s\n", mysql_error(conn));
        return 0;
    }

    MYSQL_RES *res = mysql_store_result(conn);
    if (res == NULL) return 0;

    MYSQL_ROW row = mysql_fetch_row(res);
    if (row == NULL) {
        mysql_free_result(res);
        return 0;
    }

    int submission_id = atoi(row[0]);
    int question_id = atoi(row[1]);
    double old_score = row[2] ? atof(row[2]) : 0.0;
    int exam_id = atoi(row[3]);
    mysql_free_result(res);

    // Step 2: Calculate question score (exam total score / number of questions)
    double new_score = old_score;
    double score_adjustment = 0.0;

    if (strcmp(status, "approved") == 0) {
        snprintf(query, sizeof(query),
                 "SELECT e.total_score, COUNT(eq.id) "
                 "FROM exam e "
                 "JOIN exam_questions eq ON eq.exam_id = e.id "
                 "WHERE e.id = %d "
                 "GROUP BY e.id", exam_id);

        if (mysql_query(conn, query) == 0) {
            res = mysql_store_result(conn);
            if (res != NULL) {
                row = mysql_fetch_row(res);
                if (row != NULL) {
                    double total_score = atof(row[0]);
                    int num_questions = atoi(row[1]);
                    if (num_questions > 0) {
                        double question_score = total_score / num_questions;
                        new_score = old_score + question_score;
                        score_adjustment = question_score;
                    }
                }
                mysql_free_result(res);
            }
        }
    }

    // Step 3: Update appeals table with teacher_id, old_score, new_score
    if (strlen(escaped_comment) > 0) {
        snprintf(query, sizeof(query),
                 "UPDATE appeals SET status = '%s', teacher_response = '%s', teacher_comment = '%s', "
                 "teacher_id = %d, old_score = %.2f, new_score = %.2f, score_adjustment = %.2f, "
                 "resolved_at = NOW(), teacher_read = TRUE, student_read = FALSE WHERE id = %d",
                 status, escaped_response, escaped_comment, teacher_id, old_score, new_score, score_adjustment, appeal_id);
    } else {
        snprintf(query, sizeof(query),
                 "UPDATE appeals SET status = '%s', teacher_response = '%s', "
                 "teacher_id = %d, old_score = %.2f, new_score = %.2f, score_adjustment = %.2f, "
                 "resolved_at = NOW(), teacher_read = TRUE, student_read = FALSE WHERE id = %d",
                 status, escaped_response, teacher_id, old_score, new_score, score_adjustment, appeal_id);
    }

    if (mysql_query(conn, query)) {
        fprintf(stderr, "Review appeal failed. Error: %s\n", mysql_error(conn));
        return 0;
    }

    // Step 4: If approved, update the submission score
    if (strcmp(status, "approved") == 0 && score_adjustment > 0) {
        snprintf(query, sizeof(query),
                 "UPDATE exam_submissions SET score = %.2f WHERE id = %d",
                 new_score, submission_id);
        if (mysql_query(conn, query)) {
            fprintf(stderr, "Update submission score failed. Error: %s\n", mysql_error(conn));
        }
    }

    char *timestamp = get_current_time();
    char log_message[512];
    snprintf(log_message, sizeof(log_message),
             "Appeal %d reviewed by teacher %d: %s (old_score=%.2f, new_score=%.2f)",
             appeal_id, teacher_id, status, old_score, new_score);
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

// Get appeal history for admin
char *get_appeal_history_for_admin()
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL) return NULL;

    char query[2048];
    snprintf(query, sizeof(query),
             "SELECT a.id, e.exam_name, u.name as student_name, u.email as student_email, "
             "t.name as teacher_name, t.email as teacher_email, "
             "a.status, a.old_score, a.new_score, a.score_adjustment, "
             "a.created_at, a.resolved_at, a.reason, a.teacher_response, a.teacher_comment "
             "FROM appeals a "
             "JOIN exam_submissions s ON a.submission_id = s.id "
             "JOIN exam e ON s.exam_id = e.id "
             "JOIN user u ON a.user_id = u.id "
             "LEFT JOIN user t ON a.teacher_id = t.id "
             "ORDER BY a.created_at DESC");

    if (mysql_query(conn, query)) return NULL;
    MYSQL_RES *res = mysql_store_result(conn);
    if (res == NULL) return NULL;

    cJSON *json_array = cJSON_CreateArray();
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        cJSON *obj = cJSON_CreateObject();
        cJSON_AddNumberToObject(obj, "id", atoi(row[0]));
        cJSON_AddStringToObject(obj, "exam_name", row[1]);
        cJSON_AddStringToObject(obj, "student_name", row[2]);
        cJSON_AddStringToObject(obj, "student_email", row[3]);
        cJSON_AddStringToObject(obj, "teacher_name", row[4] ? row[4] : "");
        cJSON_AddStringToObject(obj, "teacher_email", row[5] ? row[5] : "");
        cJSON_AddStringToObject(obj, "status", row[6]);
        cJSON_AddNumberToObject(obj, "old_score", row[7] ? atof(row[7]) : 0.0);
        cJSON_AddNumberToObject(obj, "new_score", row[8] ? atof(row[8]) : 0.0);
        cJSON_AddNumberToObject(obj, "score_adjustment", row[9] ? atof(row[9]) : 0.0);
        cJSON_AddStringToObject(obj, "created_at", row[10]);
        cJSON_AddStringToObject(obj, "resolved_at", row[11] ? row[11] : "");
        cJSON_AddStringToObject(obj, "reason", row[12]);
        cJSON_AddStringToObject(obj, "teacher_response", row[13] ? row[13] : "");
        cJSON_AddStringToObject(obj, "teacher_comment", row[14] ? row[14] : "");
        cJSON_AddItemToArray(json_array, obj);
    }

    mysql_free_result(res);
    char *json_string = cJSON_Print(json_array);
    cJSON_Delete(json_array);
    return json_string;
}

