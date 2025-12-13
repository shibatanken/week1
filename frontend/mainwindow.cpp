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
{
    ui->setupUi(this);

    // Add forms to the stacked widget
    ui->stackedWidget->addWidget(signupForm);
    ui->stackedWidget->addWidget(signinForm);
    ui->stackedWidget->addWidget(classListForm);
    ui->stackedWidget->addWidget(classDetailForm);

    // Connect auth signals
    connect(signupForm, &Signup::showSignin, this, &MainWindow::showSignin);
    connect(signinForm, &Signin::showSignup, this, &MainWindow::showSignup);
    
    // Connect class signals
    connect(signinForm, &Signin::loginSuccess, this, &MainWindow::showClassList);
    connect(classListForm, &ClassList::showClassDetail, this, &MainWindow::showClassDetail);
    connect(classDetailForm, &ClassDetail::showClassList, this, &MainWindow::showClassList);

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
    ui->stackedWidget->setCurrentWidget(classListForm);
}

void MainWindow::showClassDetail(int classId) {
    classDetailForm->setClassId(classId);
    ui->stackedWidget->setCurrentWidget(classDetailForm);
}
