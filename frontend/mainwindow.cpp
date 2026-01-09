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

    ui->stackedWidget->addWidget(signupForm);
    ui->stackedWidget->addWidget(signinForm);
    ui->stackedWidget->addWidget(classListForm);
    ui->stackedWidget->addWidget(classDetailForm);
    ui->stackedWidget->addWidget(classMembersForm);
    ui->stackedWidget->addWidget(createExamForm);
    ui->stackedWidget->addWidget(homeForm);

    connect(signupForm, &Signup::showSignin, this, &MainWindow::showSignin);
    connect(signinForm, &Signin::showSignup, this, &MainWindow::showSignup);
    
    connect(signinForm, &Signin::loginSuccess, this, &MainWindow::showClassList);
    connect(classListForm, &ClassList::showClassDetail, this, &MainWindow::showClassDetail);
    connect(classDetailForm, &ClassDetail::backToClassList, this, &MainWindow::showClassList);
    
    connect(classDetailForm, &ClassDetail::openClassMembers, this, &MainWindow::showClassMembers);
    connect(classMembersForm, &ClassMembers::backToClassDetail, [this]() {
        showClassDetail(currentClassId);
    });
    
    connect(classDetailForm, &ClassDetail::openCreateExam, this, &MainWindow::showCreateExam);
    connect(createExamForm, &CreateExam::backToClassDetail, [this]() {
        showClassDetail(currentClassId);
    });
    
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

void MainWindow::showHome() {
    homeForm->updateUserInfo();
    ui->stackedWidget->setCurrentWidget(homeForm);
}
