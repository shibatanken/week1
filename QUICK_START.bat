@echo off
echo ====================================
echo QUICK START - DEMO SYSTEM
echo ====================================
echo.

echo BUOC 1: Setup Database
echo -----------------------
echo Chay lenh sau trong MySQL:
echo.
echo CREATE DATABASE IF NOT EXISTS quizz_db;
echo CREATE USER IF NOT EXISTS 'quizz'@'%%' IDENTIFIED BY 'Quizz2003@';
echo GRANT ALL PRIVILEGES ON quizz_db.* TO 'quizz'@'%%';
echo FLUSH PRIVILEGES;
echo.
pause

echo.
echo BUOC 2: Run Migrations
echo -----------------------
cd backend
mysql -u quizz -pQuizz2003@ quizz_db < src/seeder/demo_data.sql
echo Migrations completed!
pause

echo.
echo BUOC 3: Build Backend
echo -----------------------
REM Note: Requires MinGW/GCC and MySQL client libraries
make clean
make
echo Backend built!
pause

echo.
echo BUOC 4: Start Server
echo -----------------------
echo Server se chay tren port 8081
echo Nhan Ctrl+C de dung server
start "Server" cmd /k server.exe
echo.
pause

echo.
echo BUOC 5: Build Frontend
echo -----------------------
cd ..\frontend
call build.bat
echo Frontend built!
pause

echo.
echo BUOC 6: Run Frontend
echo -----------------------
echo Dang khoi dong ung dung...
cd release
start frontend.exe

echo.
echo ====================================
echo DEMO READY!
echo ====================================
echo.
echo TAI KHOAN DEMO:
echo Admin: admin@example.com / admin123
echo Teacher: teacher1@example.com / teacher123
echo Student: student1@example.com / student123
echo.

