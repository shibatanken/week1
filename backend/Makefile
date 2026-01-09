CC = gcc
CFLAGS = -Wall $(shell mysql_config --cflags) -I/usr/include
LDFLAGS = $(shell mysql_config --libs) -lcjson -lpthread

SRCS = src/main.c \
       src/db/connect-db.c \
       src/routes/routes.c \
       src/controllers/auth/auth_controller.c \
       src/controllers/class/class_controller.c \
       src/controllers/room/room_controller.c \
       src/controllers/question/question_controller.c \
       src/controllers/exam/exam_controller.c \
       src/controllers/appeal/appeal_controller.c \
       src/controllers/admin/admin_controller.c \
       src/controllers/practice/practice_controller.c \
       src/controllers/statistics/statistics_controller.c \
       src/services/auth/auth_service.c \
       src/services/class/class_service.c \
       src/services/room/room_service.c \
       src/services/question/question_service.c \
       src/services/exam/exam_service.c \
       src/services/appeal/appeal_service.c \
       src/services/admin/admin_service.c \
       src/services/practice/practice_service.c \
       src/services/statistics/statistics_service.c \
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
		echo "Running $$file"; \
		mysql -u quizz -pQuizz2003@ quizz_db < $$file; \
	done
	@echo "Migrations complete!"
