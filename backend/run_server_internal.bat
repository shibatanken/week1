@echo off
title Quizz Server - Port 8081
echo ====================================
echo QUIZZ SERVER BACKEND
echo Port: 8081
echo IP: 26.136.150.83 (Radmin VPN)
echo ====================================
echo.
wsl bash -c "cd /mnt/f/LTM/week1/backend && ./server"
echo.
echo Server da dung. Nhan phim bat ky de dong cua so...
pause

