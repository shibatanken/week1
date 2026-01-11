#!/usr/bin/env python3
# Fix BUG #1: Delete question from exam should NOT delete from question bank
# It should only unlink the relationship

import sys
sys.stdout.reconfigure(encoding='utf-8')

file_path = 'F:/LTM/week1/backend/src/controllers/exam/exam_controller.c'

with open(file_path, 'r', encoding='utf-8') as f:
    content = f.read()

# Replace handle_delete_exam_question to NOT call handle_delete_question_from_bank
old_impl = '''// Legacy mapped to bank
void handle_delete_exam_question(int client_socket, ControlMessage *msg)
{
    handle_delete_question_from_bank(client_socket, msg);
}'''

new_impl = '''// Delete question from exam (unlink only, do NOT delete from bank)
void handle_delete_exam_question(int client_socket, ControlMessage *msg)
{
    KeyValuePair pairs[10];
    int pair_count = parse_json(msg->body, pairs, 1);
    int question_id = -1;
    for (int i = 0; i < pair_count; i++) {
        if (strcmp(pairs[i].key, "question_id") == 0) question_id = atoi(pairs[i].value);
    }

    // CRITICAL FIX: Only unlink from exam, do NOT delete from question bank
    // We just need to mark this question as not part of the exam
    // The question should remain in the question bank for reuse

    // For now, we'll delete the exam_questions record only
    // This requires a new service function
    MYSQL *conn = get_db_connection();
    if (conn == NULL) {
        char response[256];
        snprintf(response, sizeof(response), "NOTIFICATION DELETE_QUESTION_FAILURE\\n{\\"message\\": \\"Database connection failed\\"}");
        write(client_socket, response, strlen(response));
        close(client_socket);
        return;
    }

    char query[512];
    snprintf(query, sizeof(query), "DELETE FROM exam_questions WHERE id = %d", question_id);

    int result = mysql_query(conn, query);
    char response[2048];
    char timestamp[50];
    time_t now = time(NULL);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%S", localtime(&now));

    if (result != 0) {
        snprintf(response, sizeof(response), "NOTIFICATION DELETE_QUESTION_FAILURE %s\\n{\\"message\\": \\"Failed to delete question from exam\\"}", timestamp);
    } else {
        snprintf(response, sizeof(response), "NOTIFICATION DELETE_QUESTION_SUCCESS %s\\n{\\"message\\": \\"Question removed from exam (still in bank)\\"}", timestamp);
    }

    write(client_socket, response, strlen(response));
    close(client_socket);
}'''

if old_impl in content:
    content = content.replace(old_impl, new_impl)
    with open(file_path, 'w', encoding='utf-8') as f:
        f.write(content)
    print("[OK] Fixed BUG #1: Delete question from exam no longer deletes from bank")
else:
    print("[SKIP] Pattern not found or already fixed")
