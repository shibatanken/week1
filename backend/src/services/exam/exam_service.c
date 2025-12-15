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