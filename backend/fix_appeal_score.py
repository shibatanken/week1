#!/usr/bin/env python3
# Update backend get_appeals_for_teacher to include current_score

file_path = 'F:/LTM/week1/backend/src/services/appeal/appeal_service.c'

with open(file_path, 'r', encoding='utf-8') as f:
    content = f.read()

# Fix 1: Update SQL query to include s.score
old_query = '''    snprintf(query, sizeof(query),
             "SELECT a.id, a.submission_id, a.question_id, q.content as question_content, "
             "a.reason, a.status, a.teacher_response, a.teacher_comment, a.score_adjustment, "
             "a.created_at, a.resolved_at, e.exam_name, u.name as student_name, u.email "
             "FROM appeals a "
             "JOIN exam_questions q ON a.question_id = q.id "
             "JOIN exam_submissions s ON a.submission_id = s.id "
             "JOIN exam e ON s.exam_id = e.id "
             "JOIN class c ON e.class_id = c.id "
             "JOIN user u ON a.user_id = u.id "
             "WHERE c.teacher_id = %d ORDER BY a.created_at DESC", teacher_id);'''

new_query = '''    snprintf(query, sizeof(query),
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
             "WHERE c.teacher_id = %d ORDER BY a.created_at DESC", teacher_id);'''

# Fix 2: Add current_score to JSON object
old_json = '''        cJSON_AddStringToObject(obj, "exam_name", row[11]);
        cJSON_AddStringToObject(obj, "student_name", row[12]);
        cJSON_AddStringToObject(obj, "student_email", row[13]);
        cJSON_AddItemToArray(json_array, obj);'''

new_json = '''        cJSON_AddStringToObject(obj, "exam_name", row[11]);
        cJSON_AddStringToObject(obj, "student_name", row[12]);
        cJSON_AddStringToObject(obj, "student_email", row[13]);
        cJSON_AddNumberToObject(obj, "current_score", row[14] ? atof(row[14]) : 0.0);
        cJSON_AddItemToArray(json_array, obj);'''

# Also update get_my_appeals
old_query_student = '''    snprintf(query, sizeof(query),
             "SELECT a.id, a.submission_id, a.question_id, q.content as question_content, "
             "a.reason, a.status, a.teacher_response, a.teacher_comment, a.score_adjustment, "
             "a.created_at, a.resolved_at, e.exam_name "
             "FROM appeals a "
             "JOIN exam_questions q ON a.question_id = q.id "
             "JOIN exam_submissions s ON a.submission_id = s.id "
             "JOIN exam e ON s.exam_id = e.id "
             "WHERE a.user_id = %d ORDER BY a.created_at DESC", user_id);'''

new_query_student = '''    snprintf(query, sizeof(query),
             "SELECT a.id, a.submission_id, a.question_id, q.content as question_content, "
             "a.reason, a.status, a.teacher_response, a.teacher_comment, a.score_adjustment, "
             "a.created_at, a.resolved_at, e.exam_name, s.score as current_score "
             "FROM appeals a "
             "JOIN exam_questions q ON a.question_id = q.id "
             "JOIN exam_submissions s ON a.submission_id = s.id "
             "JOIN exam e ON s.exam_id = e.id "
             "WHERE a.user_id = %d ORDER BY a.created_at DESC", user_id);'''

old_json_student = '''        cJSON_AddStringToObject(obj, "created_at", row[9]);
        cJSON_AddStringToObject(obj, "resolved_at", row[10] ? row[10] : "");
        cJSON_AddStringToObject(obj, "exam_name", row[11]);
        cJSON_AddItemToArray(json_array, obj);'''

new_json_student = '''        cJSON_AddStringToObject(obj, "created_at", row[9]);
        cJSON_AddStringToObject(obj, "resolved_at", row[10] ? row[10] : "");
        cJSON_AddStringToObject(obj, "exam_name", row[11]);
        cJSON_AddNumberToObject(obj, "current_score", row[12] ? atof(row[12]) : 0.0);
        cJSON_AddItemToArray(json_array, obj);'''

fixed_count = 0

if old_query in content:
    content = content.replace(old_query, new_query)
    print("[OK] Updated get_appeals_for_teacher query")
    fixed_count += 1
else:
    print("[SKIP] get_appeals_for_teacher query already updated or not found")

if old_json in content:
    content = content.replace(old_json, new_json)
    print("[OK] Updated get_appeals_for_teacher JSON")
    fixed_count += 1
else:
    print("[SKIP] get_appeals_for_teacher JSON already updated or not found")

if old_query_student in content:
    content = content.replace(old_query_student, new_query_student)
    print("[OK] Updated get_my_appeals query")
    fixed_count += 1
else:
    print("[SKIP] get_my_appeals query already updated or not found")

if old_json_student in content:
    content = content.replace(old_json_student, new_json_student)
    print("[OK] Updated get_my_appeals JSON")
    fixed_count += 1
else:
    print("[SKIP] get_my_appeals JSON already updated or not found")

with open(file_path, 'w', encoding='utf-8') as f:
    f.write(content)

print(f"\n[DONE] Applied {fixed_count}/4 fixes to appeal_service.c")
