@echo off
echo ====================================
echo STARTING SERVER IN NEW WINDOW
echo ====================================
echo.
echo Server se chay trong cua so CMD rieng...
echo Nhan Ctrl+C de dung server
echo.
start "Quizz Server - Port 8081" cmd /k "%~dp0run_server_internal.bat"
