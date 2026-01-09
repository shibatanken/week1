#include "statistics_service.h"
#include "../../db/connect-db.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>

char *get_exam_statistics(int exam_id)
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL) return NULL;

    char query[512];
    
    // Get score distribution
    snprintf(query, sizeof(query),
             "SELECT score FROM exam_submissions WHERE exam_id = %d AND status = 'submitted'",
             exam_id);

    if (mysql_query(conn, query)) return NULL;
    MYSQL_RES *res = mysql_store_result(conn);
    if (res == NULL) return NULL;

    // Score ranges: 0-10, 10-20, ..., 90-100
    int score_ranges[10] = {0};
    int total_students = 0;
    double total_score = 0;
    double min_score = 100, max_score = 0;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        double score = atof(row[0]);
        total_students++;
        total_score += score;
        if (score < min_score) min_score = score;
        if (score > max_score) max_score = score;

        int range_index = (int)(score / 10);
        if (range_index >= 10) range_index = 9;
        if (range_index < 0) range_index = 0;
        score_ranges[range_index]++;
    }
    mysql_free_result(res);

    cJSON *obj = cJSON_CreateObject();
    cJSON_AddNumberToObject(obj, "exam_id", exam_id);
    cJSON_AddNumberToObject(obj, "total_students", total_students);
    cJSON_AddNumberToObject(obj, "average_score", total_students > 0 ? total_score / total_students : 0);
    cJSON_AddNumberToObject(obj, "min_score", total_students > 0 ? min_score : 0);
    cJSON_AddNumberToObject(obj, "max_score", max_score);

    cJSON *distribution = cJSON_CreateObject();
    cJSON_AddNumberToObject(distribution, "0-10", score_ranges[0]);
    cJSON_AddNumberToObject(distribution, "10-20", score_ranges[1]);
    cJSON_AddNumberToObject(distribution, "20-30", score_ranges[2]);
    cJSON_AddNumberToObject(distribution, "30-40", score_ranges[3]);
    cJSON_AddNumberToObject(distribution, "40-50", score_ranges[4]);
    cJSON_AddNumberToObject(distribution, "50-60", score_ranges[5]);
    cJSON_AddNumberToObject(distribution, "60-70", score_ranges[6]);
    cJSON_AddNumberToObject(distribution, "70-80", score_ranges[7]);
    cJSON_AddNumberToObject(distribution, "80-90", score_ranges[8]);
    cJSON_AddNumberToObject(distribution, "90-100", score_ranges[9]);
    cJSON_AddItemToObject(obj, "score_distribution", distribution);

    char *json_string = cJSON_Print(obj);
    cJSON_Delete(obj);
    return json_string;
}

char *get_class_statistics(int class_id)
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL) return NULL;

    char query[1024];
    
    // Get class info and exam count
    snprintf(query, sizeof(query),
             "SELECT c.id, c.class_name, "
             "(SELECT COUNT(*) FROM exam WHERE class_id = c.id) as exam_count, "
             "(SELECT COUNT(*) FROM user_in_class WHERE class_id = c.id) as student_count, "
             "(SELECT COUNT(*) FROM questions WHERE class_id = c.id) as question_count "
             "FROM class c WHERE c.id = %d", class_id);

    if (mysql_query(conn, query)) return NULL;
    MYSQL_RES *res = mysql_store_result(conn);
    if (res == NULL || mysql_num_rows(res) == 0) {
        mysql_free_result(res);
        return NULL;
    }

    MYSQL_ROW row = mysql_fetch_row(res);
    cJSON *obj = cJSON_CreateObject();
    cJSON_AddNumberToObject(obj, "class_id", atoi(row[0]));
    cJSON_AddStringToObject(obj, "class_name", row[1]);
    cJSON_AddNumberToObject(obj, "exam_count", atoi(row[2]));
    cJSON_AddNumberToObject(obj, "student_count", atoi(row[3]));
    cJSON_AddNumberToObject(obj, "question_count", atoi(row[4]));
    mysql_free_result(res);

    // Get exam results for chart
    snprintf(query, sizeof(query),
             "SELECT e.id, e.exam_name, "
             "COALESCE(AVG(s.score), 0) as avg_score, "
             "COUNT(s.id) as submission_count "
             "FROM exam e "
             "LEFT JOIN exam_submissions s ON e.id = s.exam_id AND s.status = 'submitted' "
             "WHERE e.class_id = %d "
             "GROUP BY e.id ORDER BY e.created_at", class_id);

    if (mysql_query(conn, query) == 0) {
        res = mysql_store_result(conn);
        if (res) {
            cJSON *exams_array = cJSON_CreateArray();
            while ((row = mysql_fetch_row(res))) {
                cJSON *exam_obj = cJSON_CreateObject();
                cJSON_AddNumberToObject(exam_obj, "exam_id", atoi(row[0]));
                cJSON_AddStringToObject(exam_obj, "exam_name", row[1]);
                cJSON_AddNumberToObject(exam_obj, "average_score", atof(row[2]));
                cJSON_AddNumberToObject(exam_obj, "submission_count", atoi(row[3]));
                cJSON_AddItemToArray(exams_array, exam_obj);
            }
            cJSON_AddItemToObject(obj, "exams", exams_array);
            mysql_free_result(res);
        }
    }

    char *json_string = cJSON_Print(obj);
    cJSON_Delete(obj);
    return json_string;
}

