#!/usr/bin/env python3
# Fix join_exam bug

import re

file_path = 'F:/LTM/week1/backend/src/services/exam/exam_service.c'

with open(file_path, 'r', encoding='utf-8') as f:
    content = f.read()

# Fix 1: Change SELECT to include status
old_select = 'SELECT id FROM exam_submissions WHERE exam_id = %d AND user_id = %d'
new_select = 'SELECT id, status FROM exam_submissions WHERE exam_id = %d AND user_id = %d'
content = content.replace(old_select, new_select)

# Fix 2: Add status check logic
old_block = '''    if (res && mysql_num_rows(res) > 0) {
        // Already joined, return existing submission_id
        MYSQL_ROW row = mysql_fetch_row(res);
        int submission_id = atoi(row[0]);
        mysql_free_result(res);
        return submission_id;
    }'''

new_block = '''    if (res && mysql_num_rows(res) > 0) {
        MYSQL_ROW row = mysql_fetch_row(res);
        int submission_id = atoi(row[0]);
        const char *status = row[1];

        // CRITICAL: Only allow rejoining if status is 'in_progress'
        // If already submitted, do NOT allow joining again
        if (strcmp(status, "submitted") == 0) {
            fprintf(stderr, "User %d attempted to rejoin submitted exam %d\\n", user_id, exam_id);
            mysql_free_result(res);
            return 0; // Return 0 to indicate cannot join
        }

        // If in_progress, allow rejoining
        mysql_free_result(res);
        return submission_id;
    }'''

content = content.replace(old_block, new_block)

with open(file_path, 'w', encoding='utf-8', newline='\n') as f:
    f.write(content)

print("[OK] Fixed join_exam function")
