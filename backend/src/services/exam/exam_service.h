#ifndef EXAM_SERVICE_H
#define EXAM_SERVICE_H

// Exam CRUD
int create_exam(const char *exam_name, const char *description, int class_id, int time_limit, const char *start_time, const char *end_time);
char *get_exams_in_class(int class_id);
char *get_exam_detail(int exam_id);
int delete_exam(int exam_id);
int update_exam_status(int exam_id, const char *status);

// Exam Question CRUD
int add_exam_question(int exam_id, const char *content, int difficulty, int *question_id);
int add_exam_answer(int question_id, const char *content, int is_correct);
char *get_exam_questions(int exam_id);
int delete_exam_question(int question_id);

#endif // EXAM_SERVICE_H