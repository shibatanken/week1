#ifndef QUESTION_SERVICE_H
#define QUESTION_SERVICE_H

int add_question(int room_id, const char *content, int difficulty, int *question_id);
int add_answer(int question_id, const char *content, int is_true);

#endif // QUESTION_SERVICE_H
