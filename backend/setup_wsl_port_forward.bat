@echo off
echo ====================================
echo SETUP WSL PORT FORWARDING
echo ====================================
echo.
echo Script nay se:
echo 1. Forward Windows port 8081 to WSL port 8081
echo 2. Mo firewall cho port 8081
echo.
echo CAN QUYEN ADMINISTRATOR!
echo.
pause

powershell -ExecutionPolicy Bypass -File "%~dp0wsl_port_forward.ps1"

pause

