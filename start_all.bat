@echo off
echo ========================================
echo STARTING SERVER AND CLIENT
echo ========================================

REM Start server in new window
echo [1/2] Starting backend server...
start "Backend Server" cmd /k "cd /d %~dp0backend && server.exe"

REM Wait 2 seconds for server to start
timeout /t 2 /nobreak >nul

REM Start frontend client in new window
echo [2/2] Starting frontend client...
start "Frontend Client" cmd /k "cd /d %~dp0frontend && release\frontend.exe"

echo ========================================
echo Both server and client started!
echo ========================================
echo.
echo Server window: "Backend Server"
echo Client window: "Frontend Client"
echo.
pause
