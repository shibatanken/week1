#!/bin/bash

# Simple build script for testing scoring pages

echo "Building test scoring application..."

# Compile all needed files
g++ -c -pipe -O2 -std=gnu++11 -Wall -W -D_REENTRANT -fPIC -DQT_NO_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_NETWORK_LIB -DQT_CORE_LIB \
    -I. \
    -I/usr/include/x86_64-linux-gnu/qt5 \
    -I/usr/include/x86_64-linux-gnu/qt5/QtWidgets \
    -I/usr/include/x86_64-linux-gnu/qt5/QtGui \
    -I/usr/include/x86_64-linux-gnu/qt5/QtNetwork \
    -I/usr/include/x86_64-linux-gnu/qt5/QtCore \
    -I/usr/lib/x86_64-linux-gnu/qt5/mkspecs/linux-g++ \
    -o test_scoring_main.o test_scoring_main.cpp

# Generate UI files if needed
uic examresults.ui -o ui_examresults.h
uic examscores.ui -o ui_examscores.h

# Generate MOC files
moc examresults.h -o moc_examresults.cpp
moc examscores.h -o moc_examscores.cpp

# Compile MOC files
g++ -c moc_examresults.cpp -I/usr/include/x86_64-linux-gnu/qt5 -I/usr/include/x86_64-linux-gnu/qt5/QtCore -I/usr/include/x86_64-linux-gnu/qt5/QtWidgets -I/usr/include/x86_64-linux-gnu/qt5/QtNetwork -I.
g++ -c moc_examscores.cpp -I/usr/include/x86_64-linux-gnu/qt5 -I/usr/include/x86_64-linux-gnu/qt5/QtCore -I/usr/include/x86_64-linux-gnu/qt5/QtWidgets -I/usr/include/x86_64-linux-gnu/qt5/QtNetwork -I.

# Compile source files
g++ -c examresults.cpp -I/usr/include/x86_64-linux-gnu/qt5 -I/usr/include/x86_64-linux-gnu/qt5/QtCore -I/usr/include/x86_64-linux-gnu/qt5/QtWidgets -I/usr/include/x86_64-linux-gnu/qt5/QtNetwork -I/usr/include/x86_64-linux-gnu/qt5/QtGui -I. -std=gnu++11
g++ -c examscores.cpp -I/usr/include/x86_64-linux-gnu/qt5 -I/usr/include/x86_64-linux-gnu/qt5/QtCore -I/usr/include/x86_64-linux-gnu/qt5/QtWidgets -I/usr/include/x86_64-linux-gnu/qt5/QtNetwork -I/usr/include/x86_64-linux-gnu/qt5/QtGui -I. -std=gnu++11

# Link
g++ -o test_scoring test_scoring_main.o examresults.o examscores.o moc_examresults.o moc_examscores.o \
    -lQt5Widgets -lQt5Gui -lQt5Network -lQt5Core -lpthread

echo "Build complete! Run with: ./test_scoring"
