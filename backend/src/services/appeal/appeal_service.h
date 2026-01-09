#ifndef APPEAL_SERVICE_H
#define APPEAL_SERVICE_H

int submit_appeal(int submission_id, int question_id, int user_id, const char *reason);
char *get_my_appeals(int user_id);
char *get_appeals_for_teacher(int teacher_id);
int review_appeal(int appeal_id, const char *status, const char *response, double score_adj);

#endif // APPEAL_SERVICE_H

