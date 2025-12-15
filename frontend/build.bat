@echo off
echo ========================================
echo BUILDING FRONTEND
echo ========================================

REM Qt 6.10.1 paths
set PATH=F:\Qt\6.10.1\mingw_64\bin;F:\Qt\Tools\mingw1310_64\bin;%PATH%

echo [1/4] Cleaning old build...
del /Q *.o moc_* ui_* Makefile 2>nul

echo [2/4] Running qmake...
qmake frontend.pro
if errorlevel 1 (
    echo ERROR: qmake failed! Check Qt path.
    pause
    exit /b 1
)

echo [3/4] Building...
mingw32-make
if errorlevel 1 (
    echo ERROR: Build failed!
    pause
    exit /b 1
)

echo [4/4] Build successful!
echo ========================================
echo Run: release\frontend.exe
echo ========================================
pause

