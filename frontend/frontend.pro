QT       += core gui
QT += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

SOURCES += \
    home.cpp \
    main.cpp \
    mainwindow.cpp \
    signin.cpp \
    signup.cpp \
    classlist.cpp \
    classdetail.cpp \
    classmembers.cpp \
    createexam.cpp

HEADERS += \
    config.h \
    home.h \
    mainwindow.h \
    signin.h \
    signup.h \
    userdata.h \
    classlist.h \
    classdetail.h \
    classmembers.h \
    createexam.h

FORMS += \
    home.ui \
    mainwindow.ui \
    signin.ui \
    signup.ui \
    classlist.ui \
    classdetail.ui \
    classmembers.ui \
    createexam.ui

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target