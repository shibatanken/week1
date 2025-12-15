#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include "signup.h"
#include "signin.h"
#include "classlist.h"
#include "classdetail.h"
#include "classmembers.h"
#include "createexam.h"
#include "home.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void showSignup();
    void showSignin();
    void showClassList();
    void showClassDetail(int classId);
    void showClassMembers(int classId, QString className);
    void showCreateExam(int classId);
    void showHome();

private:
    Ui::MainWindow *ui;
    QTcpSocket *tcpSocket;
    Signup *signupForm;
    Signin *signinForm;
    ClassList *classListForm;
    ClassDetail *classDetailForm;
    ClassMembers *classMembersForm;
    CreateExam *createExamForm;
    Home *homeForm;
    
    int currentClassId;
};

#endif // MAINWINDOW_H