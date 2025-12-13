#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include "signup.h"
#include "signin.h"
#include "classlist.h"
#include "classdetail.h"

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

private:
    Ui::MainWindow *ui;
    QTcpSocket *tcpSocket;
    Signup *signupForm;
    Signin *signinForm;
    ClassList *classListForm;
    ClassDetail *classDetailForm;
};

#endif // MAINWINDOW_H
