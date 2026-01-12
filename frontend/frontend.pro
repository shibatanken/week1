QT       += core gui
QT += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
QMAKE_CXXFLAGS += -std=c++17
QMAKE_CXXFLAGS -= -std=gnu++1z
QMAKE_CXXFLAGS -= -std=gnu++11

SOURCES += \
    home.cpp \
    main.cpp \
    mainwindow.cpp \
    signin.cpp \
    signup.cpp \
    classlist.cpp \
    classdetail.cpp \
    classmembers.cpp \
    createexam.cpp \
    questionbank.cpp \
    examtaking.cpp \
    examlist.cpp \
    practicemode.cpp \
    appealmanager.cpp \
    appealdialog.cpp \
    admindashboard.cpp \
    statisticsview.cpp

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
    createexam.h \
    questionbank.h \
    examtaking.h \
    examlist.h \
    practicemode.h \
    appealmanager.h \
    appealdialog.h \
    admindashboard.h \
    statisticsview.h

FORMS += \
    home.ui \
    mainwindow.ui \
    signin.ui \
    signup.ui \
    classlist.ui \
    classdetail.ui \
    classmembers.ui \
    createexam.ui \
    createquestion.ui \
    examtaking.ui \
    examlist.ui \
    practicemode.ui \
    appealmanager.ui \
    appealdialog.ui \
    admindashboard.ui \
    statisticsview.ui

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
