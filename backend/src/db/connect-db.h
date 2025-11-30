#ifndef CONNECT_DB_H
#define CONNECT_DB_H

#include <mysql/mysql.h>

int connect_db();
void close_db();
MYSQL* get_db_connection();

#endif // CONNECT_DB_H