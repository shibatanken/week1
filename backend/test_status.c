#include <stdio.h>
#include <mysql/mysql.h>
#include <cjson/cJSON.h>

MYSQL *get_db_connection();
char *get_submission_status(int exam_id, int user_id);

int main() {
    char *status = get_submission_status(1, 13);
    if (status) {
        printf("Status JSON:\n%s\n", status);
        printf("Length: %ld\n", strlen(status));
        free(status);
    } else {
        printf("NULL status\n");
    }
    return 0;
}
