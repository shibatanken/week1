#ifndef EXAM_SERVICE_H
#define EXAM_SERVICE_H

// Exam CRUD
int create_exam(const char *exam_name, const char *description, int class_id, int time_limit, const char *start_time, const char *end_time);
char *get_exams_in_class(int class_id);
char *get_exam_detail(int exam_id);
int delete_exam(int exam_id);
int update_exam_status(int exam_id, const char *status);

// Question Bank (Class Level)
int add_question_to_class(int class_id, const char *content, const char *opt_a, const char *opt_b, const char *opt_c, const char *opt_d, const char *correct_option);
char *get_questions_by_class(int class_id);
int delete_question_from_class(int question_id);

// Exam Questions (Exam Level)
int add_question_to_exam(int exam_id, const char *content, const char *opt_a, const char *opt_b, const char *opt_c, const char *opt_d, const char *correct_option);
char *get_exam_questions(int exam_id);
char *get_exam_questions_for_student(int exam_id); // Không có correct_option
int import_question_from_bank(int exam_id, int question_id);

// Student Exam Flow
int start_exam(int exam_id);  // Teacher starts exam -> status = ongoing
int join_exam(int exam_id, int user_id);  // Student joins -> tạo submission
char *get_submission_status(int exam_id, int user_id);  // Kiểm tra đã join chưa
int submit_answer(int submission_id, int question_id, const char *answer);
int submit_exam(int submission_id);  // Auto-grade và tính điểm
char *get_exam_result(int submission_id);
char *get_my_exam_history(int user_id);

#endif // EXAM_SERVICE_H
