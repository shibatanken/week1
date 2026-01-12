@echo off
SET PATH=F:\Qt\Tools\mingw1310_64\bin;F:\Qt\6.10.1\mingw_64\bin;%PATH%
cd /d F:\LTM\week1\frontend
del /Q Makefile*
F:\Qt\6.10.1\mingw_64\bin\qmake.exe frontend.pro
F:\Qt\Tools\mingw1310_64\bin\mingw32-make.exe
