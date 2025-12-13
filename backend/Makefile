CC = gcc
CFLAGS = -Wall -I/opt/homebrew/opt/mysql-client/include -I/opt/homebrew/opt/cjson/include
LDFLAGS = -L/opt/homebrew/opt/mysql-client/lib -lmysqlclient -L/opt/homebrew/opt/cjson/lib -lcjson

SRCS = src/main.c \
       src/db/connect-db.c \
       src/routes/routes.c \
       src/controllers/auth/auth_controller.c \
       src/controllers/class/class_controller.c \
       src/services/auth/auth_service.c \
       src/services/class/class_service.c \
       src/utils/json_utils.c \
       src/utils/mysql_utils.c \
       src/utils/time_utils.c \
       src/utils/log_utils.c

TARGET = server

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)

migrate:
	@echo "Running migrations..."
	@for file in src/migrations/*.sql; do \
		echo "Executing $$file"; \
		mysql -u quizz -pQuizz2003@ quizz_db < $$file; \
	done
	@echo "Migrations completed!"

seed:
	@echo "Seeding demo data..."
	@mysql -u quizz -pQuizz2003@ quizz_db < src/seeder/demo_data.sql
	@echo "Seeding completed!"

.PHONY: all clean run migrate seed
