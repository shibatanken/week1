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
#include "examtaking.h"
#include "examlist.h"
#include "practicemode.h"
#include "appealmanager.h"
#include "admindashboard.h"
#include "statisticsview.h"

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
    void showEditExam(int examId);
    void showHome();
    
    // New slots
    void showExamTaking(int examId, QString examName, int timeLimit);
    void showExamList();
    void showExamResult(int submissionId);
    void showPracticeMode(int classId, QString className);
    void showAppealManager();
    void showAdminDashboard();
    void showStatistics(int examId);
    void showClassStatistics(int classId);

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
    
    // New forms
    ExamTaking *examTakingForm;
    ExamList *examListForm;
    PracticeMode *practiceModeForm;
    AppealManager *appealManagerForm;
    AdminDashboard *adminDashboardForm;
    StatisticsView *statisticsViewForm;
    
    int currentClassId;
    int currentExamId;
};

#endif // MAINWINDOW_H