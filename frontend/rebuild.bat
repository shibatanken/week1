@echo off
cd /d F:\LTM\week1\frontend
set PATH=F:\Qt\Tools\mingw1310_64\bin;F:\Qt\6.10.1\mingw_64\bin;%PATH%
echo Cleaning...
del /Q Makefile.* 2>nul
echo Running qmake...
F:\Qt\6.10.1\mingw_64\bin\qmake.exe frontend.pro
echo Compiling...
mingw32-make
echo BUILD COMPLETE
