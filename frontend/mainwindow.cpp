#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , tcpSocket(new QTcpSocket(this))
    , signupForm(new Signup(this))
    , signinForm(new Signin(this))
    , homeForm(new Home(this))
    , examRoomManagementForm(new ExamRoomManagement(this))
{
    ui->setupUi(this);

    // Add forms to the stacked widget
    ui->stackedWidget->addWidget(signupForm);
    ui->stackedWidget->addWidget(signinForm);
    ui->stackedWidget->addWidget(homeForm);
    ui->stackedWidget->addWidget(examRoomManagementForm);

    // Connect signals to slots
    connect(signupForm, &Signup::showSignin, this, &MainWindow::showSignin);
    connect(signinForm, &Signin::showSignup, this, &MainWindow::showSignup);
    connect(signinForm, &Signin::showHome, this, &MainWindow::showHome);
    connect(homeForm, &Home::showExamRoomManagement, this, &MainWindow::showExamRoomManagement);
    connect(homeForm, &Home::logout, this, &MainWindow::showSignin);
    connect(examRoomManagementForm, &ExamRoomManagement::logout, this, &MainWindow::showSignin);
    connect(examRoomManagementForm, &ExamRoomManagement::showHome, this, &MainWindow::showHome);

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

void MainWindow::showHome() {
    homeForm->updateUserInfo();
    ui->stackedWidget->setCurrentWidget(homeForm);
}

void MainWindow::showExamRoomManagement() {
    ui->stackedWidget->setCurrentWidget(examRoomManagementForm);
}