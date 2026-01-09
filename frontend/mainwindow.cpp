#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "userdata.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , tcpSocket(new QTcpSocket(this))
    , signupForm(new Signup(this))
    , signinForm(new Signin(this))
    , classListForm(new ClassList(this))
    , classDetailForm(new ClassDetail(this))
    , classMembersForm(new ClassMembers(this))
    , createExamForm(new CreateExam(this))
    , homeForm(new Home(this))
    , examTakingForm(new ExamTaking(this))
    , examListForm(new ExamList(this))
    , practiceModeForm(new PracticeMode(this))
    , appealManagerForm(new AppealManager(this))
    , adminDashboardForm(new AdminDashboard(this))
    , statisticsViewForm(new StatisticsView(this))
    , currentClassId(-1)
    , currentExamId(-1)
{
    ui->setupUi(this);

    ui->stackedWidget->addWidget(signupForm);
    ui->stackedWidget->addWidget(signinForm);
    ui->stackedWidget->addWidget(classListForm);
    ui->stackedWidget->addWidget(classDetailForm);
    ui->stackedWidget->addWidget(classMembersForm);
    ui->stackedWidget->addWidget(createExamForm);
    ui->stackedWidget->addWidget(homeForm);
    ui->stackedWidget->addWidget(examTakingForm);
    ui->stackedWidget->addWidget(examListForm);
    ui->stackedWidget->addWidget(practiceModeForm);
    ui->stackedWidget->addWidget(appealManagerForm);
    ui->stackedWidget->addWidget(adminDashboardForm);
    ui->stackedWidget->addWidget(statisticsViewForm);

    // Auth connections
    connect(signupForm, &Signup::showSignin, this, &MainWindow::showSignin);
    connect(signinForm, &Signin::showSignup, this, &MainWindow::showSignup);
    connect(signinForm, &Signin::loginSuccess, [this]() {
        QString role = UserData::instance().getRole();
        if (role == "admin") {
            showAdminDashboard();
        } else {
            showClassList();
        }
    });
    
    // Class connections
    connect(classListForm, &ClassList::showClassDetail, this, &MainWindow::showClassDetail);
    connect(classListForm, &ClassList::showAppealManager, this, &MainWindow::showAppealManager);
    connect(classListForm, &ClassList::showAdminDashboard, this, &MainWindow::showAdminDashboard);
    connect(classDetailForm, &ClassDetail::backToClassList, this, &MainWindow::showClassList);
    connect(classDetailForm, &ClassDetail::openClassMembers, this, &MainWindow::showClassMembers);
    connect(classMembersForm, &ClassMembers::backToClassDetail, [this]() {
        showClassDetail(currentClassId);
    });
    connect(classDetailForm, &ClassDetail::openCreateExam, this, &MainWindow::showCreateExam);
    connect(classDetailForm, &ClassDetail::openExamDetail, this, &MainWindow::showEditExam);
    connect(classDetailForm, &ClassDetail::startExamForStudent, this, &MainWindow::showExamTaking);
    connect(classDetailForm, &ClassDetail::openPracticeMode, this, &MainWindow::showPracticeMode);
    connect(createExamForm, &CreateExam::backToClassDetail, [this]() {
        showClassDetail(currentClassId);
    });
    
    // Exam connections
    connect(examTakingForm, &ExamTaking::backToExamList, [this]() {
        showClassDetail(currentClassId);
    });
    connect(examTakingForm, &ExamTaking::examFinished, this, &MainWindow::showExamResult);
    connect(examTakingForm, &ExamTaking::openAppeal, [this](int submissionId, int examId, QString examName) {
        Q_UNUSED(examId);
        Q_UNUSED(examName);
        appealManagerForm->setSubmissionId(submissionId);
        appealManagerForm->setMode(false); // Student mode
        appealManagerForm->loadAppeals();
        ui->stackedWidget->setCurrentWidget(appealManagerForm);
    });
    connect(examListForm, &ExamList::backToClassList, this, &MainWindow::showClassList);
    connect(examListForm, &ExamList::startExam, this, &MainWindow::showExamTaking);
    connect(examListForm, &ExamList::viewExamResult, this, &MainWindow::showExamResult);
    
    // Practice connections
    connect(practiceModeForm, &PracticeMode::backToClassDetail, [this]() {
        showClassDetail(currentClassId);
    });
    
    // Appeal connections
    connect(appealManagerForm, &AppealManager::backPressed, this, &MainWindow::showClassList);
    
    // Admin connections
    connect(adminDashboardForm, &AdminDashboard::logout, this, &MainWindow::showSignin);
    
    // Statistics connections
    connect(statisticsViewForm, &StatisticsView::backPressed, [this]() {
        showClassDetail(currentClassId);
    });
    
    // Logout connections
    connect(homeForm, &Home::logout, this, &MainWindow::showSignin);
    connect(classListForm, &ClassList::logout, this, &MainWindow::showSignin);

    ui->stackedWidget->setCurrentWidget(signinForm);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::showSignup() {
    ui->stackedWidget->setCurrentWidget(signupForm);
}

void MainWindow::showSignin() {
    UserData::instance().reset();
    ui->stackedWidget->setCurrentWidget(signinForm);
}

void MainWindow::showClassList() {
    classListForm->loadClassList();
    ui->stackedWidget->setCurrentWidget(classListForm);
}

void MainWindow::showClassDetail(int classId) {
    currentClassId = classId;
    classDetailForm->setClassId(classId);
    ui->stackedWidget->setCurrentWidget(classDetailForm);
}

void MainWindow::showClassMembers(int classId, QString className) {
    currentClassId = classId;
    classMembersForm->setClassInfo(classId, className);
    ui->stackedWidget->setCurrentWidget(classMembersForm);
}

void MainWindow::showCreateExam(int classId) {
    currentClassId = classId;
    createExamForm->setClassId(classId);
    ui->stackedWidget->setCurrentWidget(createExamForm);
}

void MainWindow::showEditExam(int examId) {
    currentExamId = examId;
    createExamForm->setExamId(examId);
    ui->stackedWidget->setCurrentWidget(createExamForm);
}

void MainWindow::showHome() {
    homeForm->updateUserInfo();
    ui->stackedWidget->setCurrentWidget(homeForm);
}

void MainWindow::showExamTaking(int examId, QString examName, int timeLimit) {
    currentExamId = examId;
    examTakingForm->setExamInfo(examId, examName, timeLimit);
    examTakingForm->startExam();
    ui->stackedWidget->setCurrentWidget(examTakingForm);
}

void MainWindow::showExamList() {
    examListForm->loadExams();
    ui->stackedWidget->setCurrentWidget(examListForm);
}

void MainWindow::showExamResult(int submissionId) {
    // Could load result into a dedicated view
    Q_UNUSED(submissionId);
    showExamList();
}

void MainWindow::showPracticeMode(int classId, QString className) {
    currentClassId = classId;
    practiceModeForm->setClassInfo(classId, className);
    ui->stackedWidget->setCurrentWidget(practiceModeForm);
}

void MainWindow::showAppealManager() {
    bool isTeacher = UserData::instance().isTeacher();
    appealManagerForm->setMode(isTeacher);
    appealManagerForm->loadAppeals();
    ui->stackedWidget->setCurrentWidget(appealManagerForm);
}

void MainWindow::showAdminDashboard() {
    adminDashboardForm->loadData();
    ui->stackedWidget->setCurrentWidget(adminDashboardForm);
}

void MainWindow::showStatistics(int examId) {
    currentExamId = examId;
    statisticsViewForm->loadExamStatistics(examId);
    ui->stackedWidget->setCurrentWidget(statisticsViewForm);
}

void MainWindow::showClassStatistics(int classId) {
    currentClassId = classId;
    statisticsViewForm->loadClassStatistics(classId);
    ui->stackedWidget->setCurrentWidget(statisticsViewForm);
}
