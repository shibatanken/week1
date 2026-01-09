#!/bin/bash
cd /mnt/f/LTM/week1/backend
for file in src/migrations/*.sql; do
    echo "Running $file"
    mysql -u quizz -pQuizz2003@ quizz_db < "$file"
done
echo "Loading demo data..."
mysql -u quizz -pQuizz2003@ quizz_db < src/seeder/demo_data.sql
echo "Done!"

