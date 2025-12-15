#include "class_service.h"
#include "../../db/connect-db.h"
#include "../../utils/mysql_utils.h"
#include "../../utils/time_utils.h"
#include "../../utils/log_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>

// Tạo lớp học mới (chỉ teacher)
int create_class(const char *class_name, const char *description, int teacher_id)
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL)
    {
        fprintf(stderr, "Database connection failed.\n");
        return 0;
    }

    char query[512];
    snprintf(query, sizeof(query),
             "INSERT INTO class (class_name, description, teacher_id) VALUES ('%s', '%s', %d)",
             class_name, description, teacher_id);

    if (mysql_query(conn, query))
    {
        fprintf(stderr, "Create class failed. Error: %s\n", mysql_error(conn));
        return 0;
    }

    // Log activity
    char *timestamp = get_current_time();
    snprintf(query, sizeof(query),
             "INSERT INTO log (log_content, log_time) VALUES ('Teacher %d created class: %s', '%s')",
             teacher_id, class_name, timestamp);
    if (mysql_query(conn, query))
    {
        fprintf(stderr, "Logging failed. Error: %s\n", mysql_error(conn));
    }

    char log_message[256];
    snprintf(log_message, sizeof(log_message), "Teacher %d created class: %s", teacher_id, class_name);
    log_to_file(log_message, timestamp);
    free(timestamp);

    return 1;
}

// Lấy danh sách tất cả lớp học
char *get_all_classes()
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL)
    {
        fprintf(stderr, "Database connection failed.\n");
        return NULL;
    }

    const char *query = "SELECT c.id, c.class_name, c.description, c.teacher_id, u.name AS teacher_name, "
                        "c.created_at, COUNT(uc.user_id) AS student_count "
                        "FROM class c "
                        "JOIN user u ON c.teacher_id = u.id "
                        "LEFT JOIN user_in_class uc ON c.id = uc.class_id "
                        "GROUP BY c.id "
                        "ORDER BY c.created_at DESC";

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

    cJSON *json_array = cJSON_CreateArray();
    MYSQL_ROW row;

    while ((row = mysql_fetch_row(res)))
    {
        cJSON *class_obj = cJSON_CreateObject();
        cJSON_AddNumberToObject(class_obj, "id", atoi(row[0]));
        cJSON_AddStringToObject(class_obj, "class_name", row[1]);
        cJSON_AddStringToObject(class_obj, "description", row[2] ? row[2] : "");
        cJSON_AddNumberToObject(class_obj, "teacher_id", atoi(row[3]));
        cJSON_AddStringToObject(class_obj, "teacher_name", row[4]);
        cJSON_AddStringToObject(class_obj, "created_at", row[5]);
        cJSON_AddNumberToObject(class_obj, "student_count", atoi(row[6]));
        cJSON_AddItemToArray(json_array, class_obj);
    }

    mysql_free_result(res);

    char *json_string = cJSON_Print(json_array);
    cJSON_Delete(json_array);

    return json_string;
}

// Lấy danh sách lớp do teacher tạo
char *get_classes_by_teacher(int teacher_id)
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL) return NULL;

    char query[512];
    snprintf(query, sizeof(query),
        "SELECT c.id, c.class_name, c.description, c.teacher_id, u.name AS teacher_name, "
        "c.created_at, COUNT(uc.user_id) AS student_count "
        "FROM class c "
        "JOIN user u ON c.teacher_id = u.id "
        "LEFT JOIN user_in_class uc ON c.id = uc.class_id "
        "WHERE c.teacher_id = %d "
        "GROUP BY c.id "
        "ORDER BY c.created_at DESC", teacher_id);

    if (mysql_query(conn, query)) return NULL;

    MYSQL_RES *res = mysql_store_result(conn);
    if (res == NULL || mysql_num_rows(res) == 0) {
        mysql_free_result(res);
        return NULL;
    }

    cJSON *json_array = cJSON_CreateArray();
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        cJSON *class_obj = cJSON_CreateObject();
        cJSON_AddNumberToObject(class_obj, "id", atoi(row[0]));
        cJSON_AddStringToObject(class_obj, "class_name", row[1]);
        cJSON_AddStringToObject(class_obj, "description", row[2] ? row[2] : "");
        cJSON_AddNumberToObject(class_obj, "teacher_id", atoi(row[3]));
        cJSON_AddStringToObject(class_obj, "teacher_name", row[4]);
        cJSON_AddStringToObject(class_obj, "created_at", row[5]);
        cJSON_AddNumberToObject(class_obj, "student_count", atoi(row[6]));
        cJSON_AddItemToArray(json_array, class_obj);
    }
    mysql_free_result(res);

    char *json_string = cJSON_Print(json_array);
    cJSON_Delete(json_array);
    return json_string;
}

// Lấy danh sách lớp mà student đã join
char *get_classes_by_student(int student_id)
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL) return NULL;

    char query[512];
    snprintf(query, sizeof(query),
        "SELECT c.id, c.class_name, c.description, c.teacher_id, u.name AS teacher_name, "
        "c.created_at, COUNT(uc2.user_id) AS student_count "
        "FROM class c "
        "JOIN user u ON c.teacher_id = u.id "
        "JOIN user_in_class uc ON c.id = uc.class_id AND uc.user_id = %d "
        "LEFT JOIN user_in_class uc2 ON c.id = uc2.class_id "
        "GROUP BY c.id "
        "ORDER BY c.created_at DESC", student_id);

    if (mysql_query(conn, query)) return NULL;

    MYSQL_RES *res = mysql_store_result(conn);
    if (res == NULL || mysql_num_rows(res) == 0) {
        mysql_free_result(res);
        return NULL;
    }

    cJSON *json_array = cJSON_CreateArray();
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        cJSON *class_obj = cJSON_CreateObject();
        cJSON_AddNumberToObject(class_obj, "id", atoi(row[0]));
        cJSON_AddStringToObject(class_obj, "class_name", row[1]);
        cJSON_AddStringToObject(class_obj, "description", row[2] ? row[2] : "");
        cJSON_AddNumberToObject(class_obj, "teacher_id", atoi(row[3]));
        cJSON_AddStringToObject(class_obj, "teacher_name", row[4]);
        cJSON_AddStringToObject(class_obj, "created_at", row[5]);
        cJSON_AddNumberToObject(class_obj, "student_count", atoi(row[6]));
        cJSON_AddItemToArray(json_array, class_obj);
    }
    mysql_free_result(res);

    char *json_string = cJSON_Print(json_array);
    cJSON_Delete(json_array);
    return json_string;
}
// Lấy chi tiết lớp học
char *get_class_detail(int class_id)
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL)
    {
        fprintf(stderr, "Database connection failed.\n");
        return NULL;
    }

    char query[512];
    snprintf(query, sizeof(query),
             "SELECT c.id, c.class_name, c.description, c.teacher_id, u.name AS teacher_name, c.created_at "
             "FROM class c "
             "JOIN user u ON c.teacher_id = u.id "
             "WHERE c.id = %d",
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

    MYSQL_ROW row = mysql_fetch_row(res);
    if (row == NULL)
    {
        mysql_free_result(res);
        return NULL;
    }

    cJSON *class_obj = cJSON_CreateObject();
    cJSON_AddNumberToObject(class_obj, "id", atoi(row[0]));
    cJSON_AddStringToObject(class_obj, "class_name", row[1]);
    cJSON_AddStringToObject(class_obj, "description", row[2] ? row[2] : "");
    cJSON_AddNumberToObject(class_obj, "teacher_id", atoi(row[3]));
    cJSON_AddStringToObject(class_obj, "teacher_name", row[4]);
    cJSON_AddStringToObject(class_obj, "created_at", row[5]);

    mysql_free_result(res);

    char *json_string = cJSON_Print(class_obj);
    cJSON_Delete(class_obj);

    return json_string;
}

// Thêm sinh viên vào lớp
int add_student_to_class(int user_id, int class_id)
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL)
    {
        fprintf(stderr, "Database connection failed.\n");
        return 0;
    }

    char query[256];
    snprintf(query, sizeof(query),
             "INSERT INTO user_in_class (user_id, class_id) VALUES (%d, %d)",
             user_id, class_id);

    if (mysql_query(conn, query))
    {
        fprintf(stderr, "Add student failed. Error: %s\n", mysql_error(conn));
        return 0;
    }

    // Log activity
    char *timestamp = get_current_time();
    snprintf(query, sizeof(query),
             "INSERT INTO log (log_content, log_time) VALUES ('Student %d added to class %d', '%s')",
             user_id, class_id, timestamp);
    if (mysql_query(conn, query))
    {
        fprintf(stderr, "Logging failed. Error: %s\n", mysql_error(conn));
    }

    char log_message[256];
    snprintf(log_message, sizeof(log_message), "Student %d added to class %d", user_id, class_id);
    log_to_file(log_message, timestamp);
    free(timestamp);

    return 1;
}

// Xóa sinh viên khỏi lớp
int remove_student_from_class(int user_id, int class_id)
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL)
    {
        fprintf(stderr, "Database connection failed.\n");
        return 0;
    }

    char query[256];
    snprintf(query, sizeof(query),
             "DELETE FROM user_in_class WHERE user_id = %d AND class_id = %d",
             user_id, class_id);

    if (mysql_query(conn, query))
    {
        fprintf(stderr, "Remove student failed. Error: %s\n", mysql_error(conn));
        return 0;
    }

    // Log activity
    char *timestamp = get_current_time();
    snprintf(query, sizeof(query),
             "INSERT INTO log (log_content, log_time) VALUES ('Student %d removed from class %d', '%s')",
             user_id, class_id, timestamp);
    if (mysql_query(conn, query))
    {
        fprintf(stderr, "Logging failed. Error: %s\n", mysql_error(conn));
    }

    char log_message[256];
    snprintf(log_message, sizeof(log_message), "Student %d removed from class %d", user_id, class_id);
    log_to_file(log_message, timestamp);
    free(timestamp);

    return 1;
}

// Lấy danh sách sinh viên trong lớp
char *get_students_in_class(int class_id)
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL)
    {
        fprintf(stderr, "Database connection failed.\n");
        return NULL;
    }

    char query[512];
    snprintf(query, sizeof(query),
             "SELECT u.id, u.name, u.email, uc.joined_at "
             "FROM user u "
             "JOIN user_in_class uc ON u.id = uc.user_id "
             "WHERE uc.class_id = %d AND u.role = 'student' "
             "ORDER BY uc.joined_at DESC",
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

    int num_rows = mysql_num_rows(res);
    if (num_rows == 0)
    {
        mysql_free_result(res);
        return NULL;
    }

    cJSON *json_array = cJSON_CreateArray();
    MYSQL_ROW row;

    while ((row = mysql_fetch_row(res)))
    {
        cJSON *student_obj = cJSON_CreateObject();
        cJSON_AddNumberToObject(student_obj, "id", atoi(row[0]));
        cJSON_AddStringToObject(student_obj, "name", row[1]);
        cJSON_AddStringToObject(student_obj, "email", row[2]);
        cJSON_AddStringToObject(student_obj, "joined_at", row[3]);
        cJSON_AddItemToArray(json_array, student_obj);
    }

    mysql_free_result(res);

    char *json_string = cJSON_Print(json_array);
    cJSON_Delete(json_array);

    return json_string;
}

// Lấy danh sách sinh viên chưa có trong lớp
char *get_students_not_in_class(int class_id)
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL)
    {
        fprintf(stderr, "Database connection failed.\n");
        return NULL;
    }

    char query[512];
    snprintf(query, sizeof(query),
             "SELECT u.id, u.name, u.email "
             "FROM user u "
             "LEFT JOIN user_in_class uc ON u.id = uc.user_id AND uc.class_id = %d "
             "WHERE uc.user_id IS NULL AND u.role = 'student' "
             "ORDER BY u.name",
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

    int num_rows = mysql_num_rows(res);
    if (num_rows == 0)
    {
        mysql_free_result(res);
        return NULL;
    }

    cJSON *json_array = cJSON_CreateArray();
    MYSQL_ROW row;

    while ((row = mysql_fetch_row(res)))
    {
        cJSON *student_obj = cJSON_CreateObject();
        cJSON_AddNumberToObject(student_obj, "id", atoi(row[0]));
        cJSON_AddStringToObject(student_obj, "name", row[1]);
        cJSON_AddStringToObject(student_obj, "email", row[2]);
        cJSON_AddItemToArray(json_array, student_obj);
    }

    mysql_free_result(res);

    char *json_string = cJSON_Print(json_array);
    cJSON_Delete(json_array);

    return json_string;
}

// Xóa lớp học (chỉ teacher sở hữu)
int delete_class(int class_id, int teacher_id)
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL)
    {
        fprintf(stderr, "Database connection failed.\n");
        return 0;
    }

    // Kiểm tra quyền sở hữu
    char query[256];
    snprintf(query, sizeof(query),
             "SELECT id FROM class WHERE id = %d AND teacher_id = %d",
             class_id, teacher_id);

    if (mysql_query(conn, query))
    {
        fprintf(stderr, "Query failed. Error: %s\n", mysql_error(conn));
        return 0;
    }

    MYSQL_RES *res = mysql_store_result(conn);
    if (res == NULL || mysql_num_rows(res) == 0)
    {
        mysql_free_result(res);
        fprintf(stderr, "Permission denied or class not found.\n");
        return 0;
    }
    mysql_free_result(res);

    // Xóa lớp (CASCADE sẽ tự động xóa user_in_class)
    snprintf(query, sizeof(query), "DELETE FROM class WHERE id = %d", class_id);
    if (mysql_query(conn, query))
    {
        fprintf(stderr, "Delete class failed. Error: %s\n", mysql_error(conn));
        return 0;
    }

    // Log activity
    char *timestamp = get_current_time();
    snprintf(query, sizeof(query),
             "INSERT INTO log (log_content, log_time) VALUES ('Teacher %d deleted class %d', '%s')",
             teacher_id, class_id, timestamp);
    if (mysql_query(conn, query))
    {
        fprintf(stderr, "Logging failed. Error: %s\n", mysql_error(conn));
    }

    char log_message[256];
    snprintf(log_message, sizeof(log_message), "Teacher %d deleted class %d", teacher_id, class_id);
    log_to_file(log_message, timestamp);
    free(timestamp);

    return 1;
}

