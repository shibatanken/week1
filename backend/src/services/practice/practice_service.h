#ifndef PRACTICE_SERVICE_H
#define PRACTICE_SERVICE_H

int start_practice(int user_id, int class_id, int num_questions);
char *get_practice_questions(int session_id);
int submit_practice_answer(int session_id, int question_id, const char *answer);
int finish_practice(int session_id);
char *get_practice_result(int session_id);
char *get_practice_history(int user_id);

#endif // PRACTICE_SERVICE_H

