QT       += core gui
QT += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    signin.cpp \
    signup.cpp

HEADERS += \
    config.h \
    mainwindow.h \
    signin.h \
    signup.h \
    userdata.h

FORMS += \
    mainwindow.ui \
    signin.ui \
    signup.ui

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
