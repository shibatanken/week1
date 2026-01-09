#include "exam_service.h"
#include "../../db/connect-db.h"
#include "../../utils/mysql_utils.h"
#include "../../utils/time_utils.h"
#include "../../utils/log_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>

// Create exam
int create_exam(const char *exam_name, const char *description, int class_id, int time_limit, const char *start_time, const char *end_time)
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL)
    {
        fprintf(stderr, "Database connection failed.\n");
        return 0;
    }

    char query[1024];
    
    if (start_time != NULL && end_time != NULL && strlen(start_time) > 0 && strlen(end_time) > 0)
    {
        char start_mysql[64], end_mysql[64];
        strncpy(start_mysql, start_time, sizeof(start_mysql) - 1);
        strncpy(end_mysql, end_time, sizeof(end_mysql) - 1);
        
        for (int i = 0; start_mysql[i]; i++) { if (start_mysql[i] == 'T') start_mysql[i] = ' '; }
        for (int i = 0; end_mysql[i]; i++) { if (end_mysql[i] == 'T') end_mysql[i] = ' '; }

        snprintf(query, sizeof(query),
                 "INSERT INTO exam (exam_name, description, class_id, time_limit, start_time, end_time, status) "
                 "VALUES ('%s', '%s', %d, %d, '%s', '%s', 'draft')",
                 exam_name, description, class_id, time_limit, start_mysql, end_mysql);
    }
    else
    {
        snprintf(query, sizeof(query),
                 "INSERT INTO exam (exam_name, description, class_id, time_limit, status) "
                 "VALUES ('%s', '%s', %d, %d, 'draft')",
                 exam_name, description, class_id, time_limit);
    }

    if (mysql_query(conn, query))
    {
        fprintf(stderr, "Create exam failed. Error: %s\n", mysql_error(conn));
        return 0;
    }

    char *timestamp = get_current_time();
    char log_message[256];
    snprintf(log_message, sizeof(log_message), "Created exam: %s in class %d", exam_name, class_id);
    log_to_file(log_message, timestamp);
    free(timestamp);

    return (int)mysql_insert_id(conn);
}

// Get exams in class
char *get_exams_in_class(int class_id)
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL) return NULL;

    char query[512];
    snprintf(query, sizeof(query),
             "SELECT e.id, e.exam_name, e.description, e.time_limit, e.status, "
             "e.start_time, e.end_time, e.created_at, "
             "(SELECT COUNT(*) FROM exam_questions WHERE exam_id = e.id) AS question_count "
             "FROM exam e WHERE e.class_id = %d ORDER BY e.created_at DESC",
             class_id);

    if (mysql_query(conn, query))
    {
        fprintf(stderr, "Query failed. Error: %s\n", mysql_error(conn));
        return NULL;
    }

    MYSQL_RES *res = mysql_store_result(conn);
    if (res == NULL) return NULL;

    cJSON *json_array = cJSON_CreateArray();
    MYSQL_ROW row;

    while ((row = mysql_fetch_row(res)))
    {
        cJSON *exam_obj = cJSON_CreateObject();
        cJSON_AddNumberToObject(exam_obj, "id", atoi(row[0]));
        cJSON_AddStringToObject(exam_obj, "exam_name", row[1]);
        cJSON_AddStringToObject(exam_obj, "description", row[2] ? row[2] : "");
        cJSON_AddNumberToObject(exam_obj, "time_limit", atoi(row[3]));
        cJSON_AddStringToObject(exam_obj, "status", row[4]);
        cJSON_AddStringToObject(exam_obj, "start_time", row[5] ? row[5] : "");
        cJSON_AddStringToObject(exam_obj, "end_time", row[6] ? row[6] : "");
        cJSON_AddStringToObject(exam_obj, "created_at", row[7]);
        cJSON_AddNumberToObject(exam_obj, "question_count", atoi(row[8]));
        cJSON_AddItemToArray(json_array, exam_obj);
    }

    mysql_free_result(res);
    char *json_string = cJSON_Print(json_array);
    cJSON_Delete(json_array);
    return json_string;
}

// Get exam detail
char *get_exam_detail(int exam_id)
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL) return NULL;

    char query[512];
    snprintf(query, sizeof(query),
             "SELECT e.id, e.exam_name, e.description, e.class_id, e.time_limit, e.status, "
             "e.start_time, e.end_time, e.created_at, c.class_name "
             "FROM exam e JOIN class c ON e.class_id = c.id WHERE e.id = %d",
             exam_id);

    if (mysql_query(conn, query)) return NULL;

    MYSQL_RES *res = mysql_store_result(conn);
    if (res == NULL || mysql_num_rows(res) == 0)
    {
        mysql_free_result(res);
        return NULL;
    }

    MYSQL_ROW row = mysql_fetch_row(res);
    cJSON *exam_obj = cJSON_CreateObject();
    cJSON_AddNumberToObject(exam_obj, "id", atoi(row[0]));
    cJSON_AddStringToObject(exam_obj, "exam_name", row[1]);
    cJSON_AddStringToObject(exam_obj, "description", row[2] ? row[2] : "");
    cJSON_AddNumberToObject(exam_obj, "class_id", atoi(row[3]));
    cJSON_AddNumberToObject(exam_obj, "time_limit", atoi(row[4]));
    cJSON_AddStringToObject(exam_obj, "status", row[5]);
    cJSON_AddStringToObject(exam_obj, "start_time", row[6] ? row[6] : "");
    cJSON_AddStringToObject(exam_obj, "end_time", row[7] ? row[7] : "");
    cJSON_AddStringToObject(exam_obj, "created_at", row[8]);
    cJSON_AddStringToObject(exam_obj, "class_name", row[9]);

    mysql_free_result(res);
    char *json_string = cJSON_Print(exam_obj);
    cJSON_Delete(exam_obj);
    return json_string;
}

// Delete exam
int delete_exam(int exam_id)
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL) return 0;
    char query[256];
    snprintf(query, sizeof(query), "DELETE FROM exam WHERE id = %d", exam_id);
    if (mysql_query(conn, query)) return 0;
    return 1;
}

// Update exam status
int update_exam_status(int exam_id, const char *status)
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL) return 0;
    char query[256];
    snprintf(query, sizeof(query), "UPDATE exam SET status = '%s' WHERE id = %d", status, exam_id);
    if (mysql_query(conn, query)) return 0;
    return 1;
}

// Add question to class (Bank)
int add_question_to_class(int class_id, const char *content, const char *opt_a, const char *opt_b, const char *opt_c, const char *opt_d, const char *correct_option)
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL) return 0;

    char query[4096];
    snprintf(query, sizeof(query),
             "INSERT INTO questions (class_id, content, option_a, option_b, option_c, option_d, correct_option) "
             "VALUES (%d, '%s', '%s', '%s', '%s', '%s', '%s')",
             class_id, content, opt_a, opt_b, opt_c, opt_d, correct_option);

    if (mysql_query(conn, query))
    {
        fprintf(stderr, "Add question failed. Error: %s\n", mysql_error(conn));
        return 0;
    }
    return 1;
}

// Get bank questions
char *get_questions_by_class(int class_id)
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL) return NULL;

    char query[512];
    snprintf(query, sizeof(query),
             "SELECT id, content, option_a, option_b, option_c, option_d, correct_option, created_at FROM questions WHERE class_id = %d ORDER BY created_at DESC",
             class_id);

    if (mysql_query(conn, query)) return NULL;

    MYSQL_RES *res = mysql_store_result(conn);
    if (res == NULL) return NULL;

    cJSON *json_array = cJSON_CreateArray();
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)))
    {
        cJSON *question_obj = cJSON_CreateObject();
        cJSON_AddNumberToObject(question_obj, "id", atoi(row[0]));
        cJSON_AddStringToObject(question_obj, "content", row[1]);
        cJSON_AddStringToObject(question_obj, "option_a", row[2]);
        cJSON_AddStringToObject(question_obj, "option_b", row[3]);
        cJSON_AddStringToObject(question_obj, "option_c", row[4]);
        cJSON_AddStringToObject(question_obj, "option_d", row[5]);
        cJSON_AddStringToObject(question_obj, "correct_option", row[6]);
        cJSON_AddStringToObject(question_obj, "created_at", row[7]);
        cJSON_AddItemToArray(json_array, question_obj);
    }
    mysql_free_result(res);
    char *json_string = cJSON_Print(json_array);
    cJSON_Delete(json_array);
    return json_string;
}

// Add question to exam (Exam Questions)
int add_question_to_exam(int exam_id, const char *content, const char *opt_a, const char *opt_b, const char *opt_c, const char *opt_d, const char *correct_option)
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL) return 0;

    char query[4096];
    snprintf(query, sizeof(query),
             "INSERT INTO exam_questions (exam_id, content, option_a, option_b, option_c, option_d, correct_option) "
             "VALUES (%d, '%s', '%s', '%s', '%s', '%s', '%s')",
             exam_id, content, opt_a, opt_b, opt_c, opt_d, correct_option);

    if (mysql_query(conn, query))
    {
        fprintf(stderr, "Add exam question failed. Error: %s\n", mysql_error(conn));
        return 0;
    }
    return 1;
}

// Get exam questions
char *get_exam_questions(int exam_id)
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL) return NULL;

    char query[512];
    snprintf(query, sizeof(query),
             "SELECT id, content, option_a, option_b, option_c, option_d, correct_option FROM exam_questions WHERE exam_id = %d ORDER BY created_at ASC",
             exam_id);

    if (mysql_query(conn, query)) return NULL;

    MYSQL_RES *res = mysql_store_result(conn);
    if (res == NULL) return NULL;

    cJSON *json_array = cJSON_CreateArray();
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)))
    {
        cJSON *question_obj = cJSON_CreateObject();
        cJSON_AddNumberToObject(question_obj, "id", atoi(row[0]));
        cJSON_AddStringToObject(question_obj, "content", row[1]);
        cJSON_AddStringToObject(question_obj, "option_a", row[2]);
        cJSON_AddStringToObject(question_obj, "option_b", row[3]);
        cJSON_AddStringToObject(question_obj, "option_c", row[4]);
        cJSON_AddStringToObject(question_obj, "option_d", row[5]);
        cJSON_AddStringToObject(question_obj, "correct_option", row[6]);
        cJSON_AddItemToArray(json_array, question_obj);
    }
    mysql_free_result(res);
    char *json_string = cJSON_Print(json_array);
    cJSON_Delete(json_array);
    return json_string;
}

// Import question
int import_question_from_bank(int exam_id, int question_id)
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL) return 0;

    char query[512];
    snprintf(query, sizeof(query),
             "INSERT INTO exam_questions (exam_id, content, option_a, option_b, option_c, option_d, correct_option) "
             "SELECT %d, content, option_a, option_b, option_c, option_d, correct_option FROM questions WHERE id = %d",
             exam_id, question_id);

    if (mysql_query(conn, query))
    {
        fprintf(stderr, "Import question failed. Error: %s\n", mysql_error(conn));
        return 0;
    }
    return 1;
}

// Delete question from bank
int delete_question_from_class(int question_id)
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL) return 0;
    char query[256];
    snprintf(query, sizeof(query), "DELETE FROM questions WHERE id = %d", question_id);
    if (mysql_query(conn, query)) return 0;
    return 1;
}

// ============= STUDENT EXAM FLOW =============

// Get exam questions for student (without correct_option)
char *get_exam_questions_for_student(int exam_id)
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL) return NULL;

    char query[512];
    snprintf(query, sizeof(query),
             "SELECT id, content, option_a, option_b, option_c, option_d FROM exam_questions WHERE exam_id = %d ORDER BY id ASC",
             exam_id);

    if (mysql_query(conn, query)) return NULL;

    MYSQL_RES *res = mysql_store_result(conn);
    if (res == NULL) return NULL;

    cJSON *json_array = cJSON_CreateArray();
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)))
    {
        cJSON *question_obj = cJSON_CreateObject();
        cJSON_AddNumberToObject(question_obj, "id", atoi(row[0]));
        cJSON_AddStringToObject(question_obj, "content", row[1]);
        cJSON_AddStringToObject(question_obj, "option_a", row[2]);
        cJSON_AddStringToObject(question_obj, "option_b", row[3]);
        cJSON_AddStringToObject(question_obj, "option_c", row[4]);
        cJSON_AddStringToObject(question_obj, "option_d", row[5]);
        cJSON_AddItemToArray(json_array, question_obj);
    }
    mysql_free_result(res);
    char *json_string = cJSON_Print(json_array);
    cJSON_Delete(json_array);
    return json_string;
}

// Teacher starts exam
int start_exam(int exam_id)
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL) return 0;
    
    char query[256];
    snprintf(query, sizeof(query), 
             "UPDATE exam SET status = 'ongoing', start_time = NOW() WHERE id = %d", 
             exam_id);
    
    if (mysql_query(conn, query)) {
        fprintf(stderr, "Start exam failed. Error: %s\n", mysql_error(conn));
        return 0;
    }
    
    char *timestamp = get_current_time();
    char log_message[256];
    snprintf(log_message, sizeof(log_message), "Exam %d started", exam_id);
    log_to_file(log_message, timestamp);
    free(timestamp);
    
    return 1;
}

// Student joins exam -> create submission
int join_exam(int exam_id, int user_id)
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL) return 0;
    
    // Check if already joined
    char query[512];
    snprintf(query, sizeof(query),
             "SELECT id FROM exam_submissions WHERE exam_id = %d AND user_id = %d",
             exam_id, user_id);
    
    if (mysql_query(conn, query)) return 0;
    MYSQL_RES *res = mysql_store_result(conn);
    
    if (res && mysql_num_rows(res) > 0) {
        // Already joined, return existing submission_id
        MYSQL_ROW row = mysql_fetch_row(res);
        int submission_id = atoi(row[0]);
        mysql_free_result(res);
        return submission_id;
    }
    mysql_free_result(res);
    
    // Create new submission
    snprintf(query, sizeof(query),
             "INSERT INTO exam_submissions (exam_id, user_id, status, started_at) VALUES (%d, %d, 'in_progress', NOW())",
             exam_id, user_id);
    
    if (mysql_query(conn, query)) {
        fprintf(stderr, "Join exam failed. Error: %s\n", mysql_error(conn));
        return 0;
    }
    
    int submission_id = (int)mysql_insert_id(conn);
    
    // Pre-create answer rows for each question
    snprintf(query, sizeof(query),
             "INSERT INTO exam_answers (submission_id, question_id) "
             "SELECT %d, id FROM exam_questions WHERE exam_id = %d",
             submission_id, exam_id);
    mysql_query(conn, query); // Ignore error if no questions
    
    // Count total questions
    snprintf(query, sizeof(query),
             "UPDATE exam_submissions SET total_questions = (SELECT COUNT(*) FROM exam_questions WHERE exam_id = %d) WHERE id = %d",
             exam_id, submission_id);
    mysql_query(conn, query);
    
    char *timestamp = get_current_time();
    char log_message[256];
    snprintf(log_message, sizeof(log_message), "User %d joined exam %d (submission %d)", user_id, exam_id, submission_id);
    log_to_file(log_message, timestamp);
    free(timestamp);
    
    return submission_id;
}

// Get submission status
char *get_submission_status(int exam_id, int user_id)
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL) return NULL;
    
    char query[512];
    snprintf(query, sizeof(query),
             "SELECT s.id, s.status, s.score, s.total_questions, s.correct_answers, s.started_at, s.submitted_at, "
             "e.exam_name, e.time_limit, e.status as exam_status "
             "FROM exam_submissions s "
             "JOIN exam e ON s.exam_id = e.id "
             "WHERE s.exam_id = %d AND s.user_id = %d",
             exam_id, user_id);
    
    if (mysql_query(conn, query)) return NULL;
    MYSQL_RES *res = mysql_store_result(conn);
    if (res == NULL || mysql_num_rows(res) == 0) {
        mysql_free_result(res);
        return NULL;
    }
    
    MYSQL_ROW row = mysql_fetch_row(res);
    cJSON *obj = cJSON_CreateObject();
    cJSON_AddNumberToObject(obj, "submission_id", atoi(row[0]));
    cJSON_AddStringToObject(obj, "status", row[1]);
    cJSON_AddNumberToObject(obj, "score", atof(row[2]));
    cJSON_AddNumberToObject(obj, "total_questions", atoi(row[3]));
    cJSON_AddNumberToObject(obj, "correct_answers", atoi(row[4]));
    cJSON_AddStringToObject(obj, "started_at", row[5] ? row[5] : "");
    cJSON_AddStringToObject(obj, "submitted_at", row[6] ? row[6] : "");
    cJSON_AddStringToObject(obj, "exam_name", row[7]);
    cJSON_AddNumberToObject(obj, "time_limit", atoi(row[8]));
    cJSON_AddStringToObject(obj, "exam_status", row[9]);
    
    mysql_free_result(res);
    char *json_string = cJSON_Print(obj);
    cJSON_Delete(obj);
    return json_string;
}

// Submit answer for a question
int submit_answer(int submission_id, int question_id, const char *answer)
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL) return 0;
    
    // Check correct answer
    char query[512];
    snprintf(query, sizeof(query),
             "SELECT correct_option FROM exam_questions WHERE id = %d", question_id);
    
    if (mysql_query(conn, query)) return 0;
    MYSQL_RES *res = mysql_store_result(conn);
    if (res == NULL || mysql_num_rows(res) == 0) {
        mysql_free_result(res);
        return 0;
    }
    
    MYSQL_ROW row = mysql_fetch_row(res);
    int is_correct = (strcasecmp(row[0], answer) == 0) ? 1 : 0;
    mysql_free_result(res);
    
    // Update or insert answer
    snprintf(query, sizeof(query),
             "INSERT INTO exam_answers (submission_id, question_id, user_answer, is_correct, answered_at) "
             "VALUES (%d, %d, '%s', %d, NOW()) "
             "ON DUPLICATE KEY UPDATE user_answer = '%s', is_correct = %d, answered_at = NOW()",
             submission_id, question_id, answer, is_correct, answer, is_correct);
    
    if (mysql_query(conn, query)) {
        fprintf(stderr, "Submit answer failed. Error: %s\n", mysql_error(conn));
        return 0;
    }
    
    return 1;
}

// Submit exam - finalize and calculate score
int submit_exam(int submission_id)
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL) return 0;
    
    char query[512];
    
    // Count correct answers
    snprintf(query, sizeof(query),
             "UPDATE exam_submissions SET "
             "correct_answers = (SELECT COUNT(*) FROM exam_answers WHERE submission_id = %d AND is_correct = 1), "
             "score = (SELECT COUNT(*) FROM exam_answers WHERE submission_id = %d AND is_correct = 1) * 100.0 / "
             "NULLIF((SELECT COUNT(*) FROM exam_answers WHERE submission_id = %d), 0), "
             "status = 'submitted', submitted_at = NOW() "
             "WHERE id = %d",
             submission_id, submission_id, submission_id, submission_id);
    
    if (mysql_query(conn, query)) {
        fprintf(stderr, "Submit exam failed. Error: %s\n", mysql_error(conn));
        return 0;
    }
    
    char *timestamp = get_current_time();
    char log_message[256];
    snprintf(log_message, sizeof(log_message), "Submission %d submitted", submission_id);
    log_to_file(log_message, timestamp);
    free(timestamp);
    
    return 1;
}

// Get exam result with details
char *get_exam_result(int submission_id)
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL) return NULL;
    
    char query[1024];
    
    // Get submission info
    snprintf(query, sizeof(query),
             "SELECT s.id, s.score, s.total_questions, s.correct_answers, s.status, "
             "s.started_at, s.submitted_at, e.exam_name, u.name as student_name "
             "FROM exam_submissions s "
             "JOIN exam e ON s.exam_id = e.id "
             "JOIN user u ON s.user_id = u.id "
             "WHERE s.id = %d", submission_id);
    
    if (mysql_query(conn, query)) return NULL;
    MYSQL_RES *res = mysql_store_result(conn);
    if (res == NULL || mysql_num_rows(res) == 0) {
        mysql_free_result(res);
        return NULL;
    }
    
    MYSQL_ROW row = mysql_fetch_row(res);
    cJSON *obj = cJSON_CreateObject();
    cJSON_AddNumberToObject(obj, "submission_id", atoi(row[0]));
    cJSON_AddNumberToObject(obj, "score", atof(row[1]));
    cJSON_AddNumberToObject(obj, "total_questions", atoi(row[2]));
    cJSON_AddNumberToObject(obj, "correct_answers", atoi(row[3]));
    cJSON_AddStringToObject(obj, "status", row[4]);
    cJSON_AddStringToObject(obj, "started_at", row[5] ? row[5] : "");
    cJSON_AddStringToObject(obj, "submitted_at", row[6] ? row[6] : "");
    cJSON_AddStringToObject(obj, "exam_name", row[7]);
    cJSON_AddStringToObject(obj, "student_name", row[8]);
    mysql_free_result(res);
    
    // Get detailed answers
    snprintf(query, sizeof(query),
             "SELECT a.question_id, q.content, a.user_answer, q.correct_option, a.is_correct "
             "FROM exam_answers a "
             "JOIN exam_questions q ON a.question_id = q.id "
             "WHERE a.submission_id = %d ORDER BY q.id", submission_id);
    
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

// Get exam history for a student
char *get_my_exam_history(int user_id)
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL) return NULL;
    
    char query[512];
    snprintf(query, sizeof(query),
             "SELECT s.id, e.exam_name, c.class_name, s.score, s.total_questions, s.correct_answers, "
             "s.status, s.started_at, s.submitted_at "
             "FROM exam_submissions s "
             "JOIN exam e ON s.exam_id = e.id "
             "JOIN class c ON e.class_id = c.id "
             "WHERE s.user_id = %d ORDER BY s.started_at DESC", user_id);
    
    if (mysql_query(conn, query)) return NULL;
    MYSQL_RES *res = mysql_store_result(conn);
    if (res == NULL) return NULL;
    
    cJSON *json_array = cJSON_CreateArray();
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        cJSON *obj = cJSON_CreateObject();
        cJSON_AddNumberToObject(obj, "submission_id", atoi(row[0]));
        cJSON_AddStringToObject(obj, "exam_name", row[1]);
        cJSON_AddStringToObject(obj, "class_name", row[2]);
        cJSON_AddNumberToObject(obj, "score", atof(row[3]));
        cJSON_AddNumberToObject(obj, "total_questions", atoi(row[4]));
        cJSON_AddNumberToObject(obj, "correct_answers", atoi(row[5]));
        cJSON_AddStringToObject(obj, "status", row[6]);
        cJSON_AddStringToObject(obj, "started_at", row[7] ? row[7] : "");
        cJSON_AddStringToObject(obj, "submitted_at", row[8] ? row[8] : "");
        cJSON_AddItemToArray(json_array, obj);
    }
    
    mysql_free_result(res);
    char *json_string = cJSON_Print(json_array);
    cJSON_Delete(json_array);
    return json_string;
}