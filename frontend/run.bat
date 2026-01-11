@echo off
echo ========================================
echo RUNNING FRONTEND
echo ========================================

REM Set Qt path
set PATH=F:\Qt\6.10.1\mingw_64\bin;F:\Qt\Tools\mingw1310_64\bin;%PATH%

REM Deploy Qt DLLs (chỉ chạy 1 lần)
if not exist release\Qt6Core.dll (
    echo Deploying Qt DLLs...
    cd release
    windeployqt frontend.exe
    cd ..
)

REM Run frontend
echo Starting frontend...
cd release
start frontend.exe
cd ..

echo ========================================
echo Frontend started!
echo ========================================




