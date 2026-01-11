// CRITICAL FIXES FOR GRADING SYSTEM
// File: exam_service_fixes.c
// These are the corrected versions of submit_answer() and submit_exam()

// Submit answer for a question - FIXED VERSION
int submit_answer_FIXED(int submission_id, int question_id, const char *answer)
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL) return 0;

    // CRITICAL: Validate answer before processing
    // Don't save empty or invalid answers
    if (answer == NULL || strlen(answer) == 0) {
        fprintf(stderr, "Empty answer for question %d, skipping save\n", question_id);
        return 1; // Return success but don't save empty answer
    }

    // CRITICAL: Validate answer is one of A, B, C, D only
    if (strlen(answer) != 1 || (answer[0] != 'A' && answer[0] != 'B' && answer[0] != 'C' && answer[0] != 'D')) {
        fprintf(stderr, "Invalid answer format: '%s' (must be A, B, C, or D)\n", answer);
        return 1; // Return success but don't save invalid answer
    }

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

    // Escape answer to prevent SQL injection
    char escaped_answer[10];
    mysql_real_escape_string(conn, escaped_answer, answer, strlen(answer));

    // CRITICAL: Use UPDATE instead of INSERT ... ON DUPLICATE KEY UPDATE
    // This ensures we only update existing rows (created by join_exam)
    snprintf(query, sizeof(query),
             "UPDATE exam_answers SET user_answer = '%s', is_correct = %d, answered_at = NOW() "
             "WHERE submission_id = %d AND question_id = %d",
             escaped_answer, is_correct, submission_id, question_id);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "Submit answer failed. Error: %s\n", mysql_error(conn));
        return 0;
    }

    return 1;
}

// Submit exam - finalize and calculate score - FIXED VERSION
int submit_exam_FIXED(int submission_id)
{
    MYSQL *conn = get_db_connection();
    if (conn == NULL) return 0;

    char query[1024];

    // CRITICAL FIX: Calculate score based on TOTAL questions in exam, not just answered questions
    // This ensures:
    // - If student answers 0 questions: score = 0 / total = 0.0
    // - If student answers some questions: score = correct / total * 100
    // - Score is always out of 100% based on ALL questions in the exam
    snprintf(query, sizeof(query),
             "UPDATE exam_submissions s SET "
             "s.correct_answers = (SELECT COUNT(*) FROM exam_answers WHERE submission_id = %d AND is_correct = 1 AND user_answer IS NOT NULL), "
             "s.score = IFNULL("
             "  (SELECT COUNT(*) FROM exam_answers WHERE submission_id = %d AND is_correct = 1 AND user_answer IS NOT NULL) * 100.0 / "
             "  NULLIF(s.total_questions, 0), "
             "  0.0"
             "), "
             "s.status = 'submitted', s.submitted_at = NOW() "
             "WHERE s.id = %d",
             submission_id, submission_id, submission_id);

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
