#include "mainwindow.h"
#include "ui_mainwindow.h"

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
    , currentClassId(-1)
{
    ui->setupUi(this);

    // Add forms to the stacked widget
    ui->stackedWidget->addWidget(signupForm);
    ui->stackedWidget->addWidget(signinForm);
    ui->stackedWidget->addWidget(classListForm);
    ui->stackedWidget->addWidget(classDetailForm);
    ui->stackedWidget->addWidget(classMembersForm);
    ui->stackedWidget->addWidget(createExamForm);
    ui->stackedWidget->addWidget(homeForm);

    // Connect auth signals
    connect(signupForm, &Signup::showSignin, this, &MainWindow::showSignin);
    connect(signinForm, &Signin::showSignup, this, &MainWindow::showSignup);
    
    // Connect class signals
    connect(signinForm, &Signin::loginSuccess, this, &MainWindow::showClassList);
    connect(classListForm, &ClassList::showClassDetail, this, &MainWindow::showClassDetail);
    connect(classDetailForm, &ClassDetail::showClassList, this, &MainWindow::showClassList);
    
    // Connect class members signals
    connect(classDetailForm, &ClassDetail::showClassMembers, this, &MainWindow::showClassMembers);
    connect(classMembersForm, &ClassMembers::backToClassDetail, [this]() {
        showClassDetail(currentClassId);
    });
    
    // Connect create exam signals
    connect(classDetailForm, &ClassDetail::showCreateExam, this, &MainWindow::showCreateExam);
    connect(createExamForm, &CreateExam::backToClassDetail, [this]() {
        showClassDetail(currentClassId);
    });
    connect(createExamForm, &CreateExam::examCreated, []() {
        // Refresh classDetail sau khi tạo exam
    });
    
    // Connect home signals
    connect(homeForm, &Home::logout, this, &MainWindow::showSignin);
    connect(classListForm, &ClassList::logout, this, &MainWindow::showSignin);

    // Show the signin form initially
    ui->stackedWidget->setCurrentWidget(signinForm);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::showSignup() {
    ui->stackedWidget->setCurrentWidget(signupForm);
}

void MainWindow::showSignin() {
    ui->stackedWidget->setCurrentWidget(signinForm);
}

void MainWindow::showClassList() {
    classListForm->loadClassList();  // Sửa từ loadClasses() thành loadClassList()
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

void MainWindow::showHome() {
    homeForm->updateUserInfo();
    ui->stackedWidget->setCurrentWidget(homeForm);
}