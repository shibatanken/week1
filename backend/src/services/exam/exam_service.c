#include "exam_service.h"
#include "../../db/connect-db.h"
#include "../../utils/mysql_utils.h"
#include "../../utils/time_utils.h"
#include "../../utils/log_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>

// Tạo bài kiểm tra mới
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
        // Convert ISO format to MySQL format
        char start_mysql[64], end_mysql[64];
        strncpy(start_mysql, start_time, sizeof(start_mysql) - 1);
        strncpy(end_mysql, end_time, sizeof(end_mysql) - 1);
        
        for (int i = 0; start_mysql[i]; i++) {
            if (start_mysql[i] == 'T') start_mysql[i] = ' ';
        }
        for (int i = 0; end_mysql[i]; i++) {
            if (end_mysql[i] == 'T') end_mysql[i] = ' ';
        }

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

    // Log activity
    char *timestamp = get_current_time();
    char log_message[256];
    snprintf(log_message, sizeof(log_message), "Created exam: %s in class %d", exam_name, class_id);
    log_to_file(log_message, timestamp);
    free(timestamp);

    return (int)mysql_insert_id(conn);
}

// Lấy danh sách bài kiểm tra trong lớp
char *get_exams_in_class(int class_id)
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL)
    {
        fprintf(stderr, "Database connection failed.\n");
        return NULL;
    }

    char query[512];
    snprintf(query, sizeof(query),
             "SELECT e.id, e.exam_name, e.description, e.time_limit, e.status, "
             "e.start_time, e.end_time, e.created_at, "
             "(SELECT COUNT(*) FROM exam_question WHERE exam_id = e.id) AS question_count "
             "FROM exam e WHERE e.class_id = %d ORDER BY e.created_at DESC",
             class_id);

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

// Lấy chi tiết bài kiểm tra
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

// Xóa bài kiểm tra
int delete_exam(int exam_id)
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL) return 0;

    char query[256];
    snprintf(query, sizeof(query), "DELETE FROM exam WHERE id = %d", exam_id);

    if (mysql_query(conn, query))
    {
        fprintf(stderr, "Delete exam failed. Error: %s\n", mysql_error(conn));
        return 0;
    }

    return 1;
}

// Cập nhật trạng thái bài kiểm tra
int update_exam_status(int exam_id, const char *status)
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL) return 0;

    char query[256];
    snprintf(query, sizeof(query), "UPDATE exam SET status = '%s' WHERE id = %d", status, exam_id);

    if (mysql_query(conn, query))
    {
        fprintf(stderr, "Update exam status failed. Error: %s\n", mysql_error(conn));
        return 0;
    }

    return 1;
}

// Thêm câu hỏi vào bài kiểm tra
int add_exam_question(int exam_id, const char *content, int difficulty, int *question_id)
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL) return 0;

    char query[1024];
    snprintf(query, sizeof(query),
             "INSERT INTO exam_question (exam_id, content, difficulty) VALUES (%d, '%s', %d)",
             exam_id, content, difficulty);

    if (mysql_query(conn, query))
    {
        fprintf(stderr, "Add exam question failed. Error: %s\n", mysql_error(conn));
        return 0;
    }

    *question_id = (int)mysql_insert_id(conn);
    return 1;
}

// Thêm đáp án cho câu hỏi
int add_exam_answer(int question_id, const char *content, int is_correct)
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL) return 0;

    char query[512];
    snprintf(query, sizeof(query),
             "INSERT INTO exam_answer (question_id, content, is_correct) VALUES (%d, '%s', %d)",
             question_id, content, is_correct);

    if (mysql_query(conn, query))
    {
        fprintf(stderr, "Add exam answer failed. Error: %s\n", mysql_error(conn));
        return 0;
    }

    return 1;
}

// Lấy danh sách câu hỏi của bài kiểm tra
char *get_exam_questions(int exam_id)
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL) return NULL;

    char query[512];
    snprintf(query, sizeof(query),
             "SELECT id, content, difficulty, created_at FROM exam_question WHERE exam_id = %d ORDER BY id",
             exam_id);

    if (mysql_query(conn, query)) return NULL;

    MYSQL_RES *res = mysql_store_result(conn);
    if (res == NULL) return NULL;

    cJSON *json_array = cJSON_CreateArray();
    MYSQL_ROW row;

    while ((row = mysql_fetch_row(res)))
    {
        cJSON *question_obj = cJSON_CreateObject();
        int question_id = atoi(row[0]);
        cJSON_AddNumberToObject(question_obj, "id", question_id);
        cJSON_AddStringToObject(question_obj, "content", row[1]);
        cJSON_AddNumberToObject(question_obj, "difficulty", atoi(row[2]));
        cJSON_AddStringToObject(question_obj, "created_at", row[3]);

        // Lấy danh sách đáp án
        char answer_query[256];
        snprintf(answer_query, sizeof(answer_query),
                 "SELECT id, content, is_correct FROM exam_answer WHERE question_id = %d", question_id);

        if (mysql_query(conn, answer_query) == 0)
        {
            MYSQL_RES *answer_res = mysql_store_result(conn);
            if (answer_res != NULL)
            {
                cJSON *answers_array = cJSON_CreateArray();
                MYSQL_ROW answer_row;

                while ((answer_row = mysql_fetch_row(answer_res)))
                {
                    cJSON *answer_obj = cJSON_CreateObject();
                    cJSON_AddNumberToObject(answer_obj, "id", atoi(answer_row[0]));
                    cJSON_AddStringToObject(answer_obj, "content", answer_row[1]);
                    cJSON_AddBoolToObject(answer_obj, "is_correct", atoi(answer_row[2]));
                    cJSON_AddItemToArray(answers_array, answer_obj);
                }

                cJSON_AddItemToObject(question_obj, "answers", answers_array);
                mysql_free_result(answer_res);
            }
        }

        cJSON_AddItemToArray(json_array, question_obj);
    }

    mysql_free_result(res);

    char *json_string = cJSON_Print(json_array);
    cJSON_Delete(json_array);

    return json_string;
}

// Xóa câu hỏi
int delete_exam_question(int question_id)
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL) return 0;

    char query[256];
    snprintf(query, sizeof(query), "DELETE FROM exam_question WHERE id = %d", question_id);

    if (mysql_query(conn, query))
    {
        fprintf(stderr, "Delete exam question failed. Error: %s\n", mysql_error(conn));
        return 0;
    }

    return 1;
}

// Nộp bài kiểm tra và tính điểm
int submit_exam_answers(int exam_id, int user_id, const char *start_time, const char *end_time, const char *answers_json)
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL) return 0;

    // Parse JSON answers
    cJSON *answers_array = cJSON_Parse(answers_json);
    if (answers_array == NULL) return 0;

    // Convert ISO time to MySQL format
    char start_mysql[64], end_mysql[64];
    strncpy(start_mysql, start_time, sizeof(start_mysql) - 1);
    strncpy(end_mysql, end_time, sizeof(end_mysql) - 1);

    for (int i = 0; start_mysql[i]; i++) {
        if (start_mysql[i] == 'T') start_mysql[i] = ' ';
    }
    for (int i = 0; end_mysql[i]; i++) {
        if (end_mysql[i] == 'T') end_mysql[i] = ' ';
    }

    // Count total questions in exam
    char count_query[256];
    snprintf(count_query, sizeof(count_query),
             "SELECT COUNT(*) FROM exam_question WHERE exam_id = %d", exam_id);

    int total_questions = 0;
    if (mysql_query(conn, count_query) == 0)
    {
        MYSQL_RES *res = mysql_store_result(conn);
        if (res != NULL)
        {
            MYSQL_ROW row = mysql_fetch_row(res);
            total_questions = atoi(row[0]);
            mysql_free_result(res);
        }
    }

    // Create user_exam record first
    char insert_query[512];
    snprintf(insert_query, sizeof(insert_query),
             "INSERT INTO user_exam (exam_id, user_id, start_time, end_time, total_questions) "
             "VALUES (%d, %d, '%s', '%s', %d) "
             "ON DUPLICATE KEY UPDATE start_time='%s', end_time='%s', total_questions=%d",
             exam_id, user_id, start_mysql, end_mysql, total_questions,
             start_mysql, end_mysql, total_questions);

    if (mysql_query(conn, insert_query))
    {
        fprintf(stderr, "Insert user_exam failed. Error: %s\n", mysql_error(conn));
        cJSON_Delete(answers_array);
        return 0;
    }

    int user_exam_id = (int)mysql_insert_id(conn);
    if (user_exam_id == 0)
    {
        // Record was updated, get the existing ID
        char get_id_query[256];
        snprintf(get_id_query, sizeof(get_id_query),
                 "SELECT id FROM user_exam WHERE exam_id = %d AND user_id = %d",
                 exam_id, user_id);

        if (mysql_query(conn, get_id_query) == 0)
        {
            MYSQL_RES *res = mysql_store_result(conn);
            if (res != NULL)
            {
                MYSQL_ROW row = mysql_fetch_row(res);
                user_exam_id = atoi(row[0]);
                mysql_free_result(res);
            }
        }
    }

    // Process each answer
    int correct_count = 0;
    int wrong_count = 0;

    cJSON *answer_item;
    cJSON_ArrayForEach(answer_item, answers_array)
    {
        cJSON *exam_question_id_obj = cJSON_GetObjectItem(answer_item, "exam_question_id");
        cJSON *answer_obj = cJSON_GetObjectItem(answer_item, "answer");

        if (!cJSON_IsNumber(exam_question_id_obj) || !cJSON_IsString(answer_obj))
            continue;

        int exam_question_id = exam_question_id_obj->valueint;
        const char *selected_answer = answer_obj->valuestring;

        // Get the correct answer for this question
        // Letter "A" = answer at position 0, "B" = position 1, etc.
        char check_query[512];
        snprintf(check_query, sizeof(check_query),
                 "SELECT ea.is_correct "
                 "FROM exam_answer ea "
                 "WHERE ea.question_id = %d "
                 "ORDER BY ea.id LIMIT 26",
                 exam_question_id);

        int is_correct = 0;
        if (mysql_query(conn, check_query) == 0)
        {
            MYSQL_RES *res = mysql_store_result(conn);
            if (res != NULL)
            {
                int answer_index = selected_answer[0] - 'A'; // Convert 'A' to 0, 'B' to 1, etc.
                int current_index = 0;
                MYSQL_ROW row;

                while ((row = mysql_fetch_row(res)))
                {
                    if (current_index == answer_index)
                    {
                        is_correct = atoi(row[0]);
                        break;
                    }
                    current_index++;
                }
                mysql_free_result(res);
            }
        }

        if (is_correct)
            correct_count++;
        else
            wrong_count++;

        // Insert user answer
        char answer_insert[512];
        snprintf(answer_insert, sizeof(answer_insert),
                 "INSERT INTO user_exam_answer (user_exam_id, exam_question_id, selected_answer, is_correct) "
                 "VALUES (%d, %d, '%s', %d)",
                 user_exam_id, exam_question_id, selected_answer, is_correct);

        mysql_query(conn, answer_insert);
    }

    cJSON_Delete(answers_array);

    // Calculate score percentage
    float score = 0.0;
    if (total_questions > 0)
    {
        score = ((float)correct_count / (float)total_questions) * 100.0;
    }

    // Update user_exam with scores
    char update_query[512];
    snprintf(update_query, sizeof(update_query),
             "UPDATE user_exam SET score = %.2f, correct_count = %d, wrong_count = %d WHERE id = %d",
             score, correct_count, wrong_count, user_exam_id);

    if (mysql_query(conn, update_query))
    {
        fprintf(stderr, "Update scores failed. Error: %s\n", mysql_error(conn));
        return 0;
    }

    return user_exam_id;
}

// Lấy kết quả bài kiểm tra của sinh viên
char *get_exam_results(int exam_id, int user_id)
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL) return NULL;

    char query[512];
    snprintf(query, sizeof(query),
             "SELECT ue.id, ue.score, ue.correct_count, ue.wrong_count, ue.total_questions, "
             "ue.start_time, ue.end_time, ue.submitted_at, e.exam_name "
             "FROM user_exam ue "
             "JOIN exam e ON ue.exam_id = e.id "
             "WHERE ue.exam_id = %d AND ue.user_id = %d",
             exam_id, user_id);

    if (mysql_query(conn, query)) return NULL;

    MYSQL_RES *res = mysql_store_result(conn);
    if (res == NULL || mysql_num_rows(res) == 0)
    {
        if (res) mysql_free_result(res);
        return NULL;
    }

    MYSQL_ROW row = mysql_fetch_row(res);

    cJSON *result_obj = cJSON_CreateObject();
    cJSON_AddNumberToObject(result_obj, "user_exam_id", atoi(row[0]));
    cJSON_AddNumberToObject(result_obj, "score", atof(row[1]));
    cJSON_AddNumberToObject(result_obj, "correct_count", atoi(row[2]));
    cJSON_AddNumberToObject(result_obj, "wrong_count", atoi(row[3]));
    cJSON_AddNumberToObject(result_obj, "total_questions", atoi(row[4]));
    cJSON_AddStringToObject(result_obj, "start_time", row[5] ? row[5] : "");
    cJSON_AddStringToObject(result_obj, "end_time", row[6] ? row[6] : "");
    cJSON_AddStringToObject(result_obj, "submitted_at", row[7] ? row[7] : "");
    cJSON_AddStringToObject(result_obj, "exam_name", row[8]);

    mysql_free_result(res);

    char *json_string = cJSON_Print(result_obj);
    cJSON_Delete(result_obj);

    return json_string;
}

// Lấy danh sách điểm của tất cả sinh viên (cho giáo viên)
char *get_exam_scores(int exam_id)
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL) return NULL;

    char query[512];
    snprintf(query, sizeof(query),
             "SELECT u.id, u.name, u.email, ue.score, ue.correct_count, ue.wrong_count, "
             "ue.total_questions, ue.submitted_at "
             "FROM user_exam ue "
             "JOIN user u ON ue.user_id = u.id "
             "WHERE ue.exam_id = %d "
             "ORDER BY ue.score DESC, ue.submitted_at ASC",
             exam_id);

    if (mysql_query(conn, query)) return NULL;

    MYSQL_RES *res = mysql_store_result(conn);
    if (res == NULL) return NULL;

    cJSON *json_array = cJSON_CreateArray();
    MYSQL_ROW row;

    while ((row = mysql_fetch_row(res)))
    {
        cJSON *student_obj = cJSON_CreateObject();
        cJSON_AddNumberToObject(student_obj, "user_id", atoi(row[0]));
        cJSON_AddStringToObject(student_obj, "name", row[1]);
        cJSON_AddStringToObject(student_obj, "email", row[2]);
        cJSON_AddNumberToObject(student_obj, "score", atof(row[3]));
        cJSON_AddNumberToObject(student_obj, "correct_count", atoi(row[4]));
        cJSON_AddNumberToObject(student_obj, "wrong_count", atoi(row[5]));
        cJSON_AddNumberToObject(student_obj, "total_questions", atoi(row[6]));
        cJSON_AddStringToObject(student_obj, "submitted_at", row[7] ? row[7] : "");
        cJSON_AddItemToArray(json_array, student_obj);
    }

    mysql_free_result(res);

    char *json_string = cJSON_Print(json_array);
    cJSON_Delete(json_array);

    return json_string;
}