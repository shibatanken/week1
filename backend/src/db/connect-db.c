#include "connect-db.h"
#include <stdio.h>
#include <stdlib.h>

static MYSQL *conn;

int connect_db() {
    conn = mysql_init(NULL);
    if (conn == NULL) {
        fprintf(stderr, "mysql_init() failed\n");
        return EXIT_FAILURE;
    }

    if (mysql_real_connect(conn, "localhost", "quizz", "Quizz2003@", "quizz_db", 0, NULL, 0) == NULL) {
        fprintf(stderr, "mysql_real_connect() failed\n");
        mysql_close(conn);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void close_db() {
    if (conn != NULL) {
        mysql_close(conn);
    }
}

MYSQL* get_db_connection() {
    return conn;
}


