QT       += core gui
QT += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

SOURCES += \
    createexamroom.cpp \
    createquestion.cpp \
    examroomdialog.cpp \
    examroommanagement.cpp \
    home.cpp \
    main.cpp \
    mainwindow.cpp \
    signin.cpp \
    signup.cpp

HEADERS += \
    config.h \
    createexamroom.h \
    createquestion.h \
    examroomdialog.h \
    examroommanagement.h \
    home.h \
    mainwindow.h \
    signin.h \
    signup.h \
    userdata.h

FORMS += \
    createexamroom.ui \
    createquestion.ui \
    examroomdialog.ui \
    examroommanagement.ui \
    home.ui \
    mainwindow.ui \
    signin.ui \
    signup.ui

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
