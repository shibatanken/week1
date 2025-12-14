#ifndef ROOM_SERVICE_H
#define ROOM_SERVICE_H

char *get_all_room();
int create_room(const char *subject, const char *description, int easy, int medium, int hard, int time_limit, const char *start, const char *end);
char *get_room_question(int room_id);

#endif // ROOM_SERVICE_H