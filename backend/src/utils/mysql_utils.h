#ifndef MYSQL_UTILS_H
#define MYSQL_UTILS_H

#include <mysql/mysql.h>

void print_mysql_result(MYSQL_RES *res);
char *mysql_time_to_str(MYSQL_TIME *time);

#endif // MYSQL_UTILS_H