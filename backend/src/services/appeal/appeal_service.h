#ifndef APPEAL_SERVICE_H
#define APPEAL_SERVICE_H

int submit_appeal(int submission_id, int question_id, int user_id, const char *reason);
char *get_my_appeals(int user_id);
char *get_appeals_for_teacher(int teacher_id);
int review_appeal(int appeal_id, int teacher_id, const char *status, const char *response, const char *teacher_comment);
int get_unread_appeals_count(int user_id, int is_teacher);
int mark_appeal_as_read(int appeal_id, int user_id, int is_teacher);
char *get_appeal_history_for_admin();

#endif // APPEAL_SERVICE_H

