@echo off
echo ====================================
echo MO FIREWALL PORT 8081
echo ====================================
echo.
echo Script nay se mo port 8081 trong Windows Firewall
echo CAN QUYEN ADMIN - Nhan phim bat ky de tiep tuc...
pause

netsh advfirewall firewall add rule name="Quizz Server Port 8081" dir=in action=allow protocol=TCP localport=8081

if %ERRORLEVEL% EQU 0 (
    echo.
    echo SUCCESS! Port 8081 da duoc mo trong firewall.
) else (
    echo.
    echo ERROR! Khong the mo port. Hay chay lai voi quyền Administrator.
    echo Click phai chuot vao file -> Run as administrator
)

echo.
pause

