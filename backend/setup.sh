#!/bin/bash
sudo mysql -e "DROP DATABASE IF EXISTS quizz_db; CREATE DATABASE quizz_db;"
cd /mnt/f/LTM/week1/backend
mysql -u quizz -pQuizz2003@ quizz_db < src/migrations/create_01_user_table.sql
mysql -u quizz -pQuizz2003@ quizz_db < src/migrations/create_02_class_table.sql
mysql -u quizz -pQuizz2003@ quizz_db < src/migrations/create_03_user_in_class_table.sql
mysql -u quizz -pQuizz2003@ quizz_db < src/migrations/create_04_log_table.sql
mysql -u quizz -pQuizz2003@ quizz_db < src/migrations/create_05_exam_table.sql
mysql -u quizz -pQuizz2003@ quizz_db < src/migrations/create_06_questions_table.sql
mysql -u quizz -pQuizz2003@ quizz_db < src/migrations/create_08_exam_questions_table.sql
mysql -u quizz -pQuizz2003@ quizz_db < src/migrations/create_09_exam_submissions_table.sql
mysql -u quizz -pQuizz2003@ quizz_db < src/migrations/create_10_exam_answers_table.sql
mysql -u quizz -pQuizz2003@ quizz_db < src/migrations/create_11_practice_sessions_table.sql
mysql -u quizz -pQuizz2003@ quizz_db < src/migrations/create_12_practice_answers_table.sql
mysql -u quizz -pQuizz2003@ quizz_db < src/migrations/create_13_appeals_table.sql
mysql -u quizz -pQuizz2003@ quizz_db < src/migrations/alter_01_user_add_status.sql
echo "✅ Database ready!"

