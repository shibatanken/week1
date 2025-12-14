#include "home.h"
#include "ui_home.h"

Home::Home(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Home)
{
    ui->setupUi(this);
    
    connect(ui->examRoomButton, &QPushButton::clicked, this, &Home::on_examRoomButton_clicked);
    connect(ui->logoutButton, &QPushButton::clicked, this, &Home::on_logoutButton_clicked);
}

Home::~Home() {
    delete ui;
}

void Home::updateUserInfo() {
    QString userName = UserData::instance().getUserName();
    QString email = UserData::instance().getEmail();
    
    ui->welcomeLabel->setText("Chào mừng, " + userName + "!");
    ui->emailLabel->setText("Email: " + email);
}

void Home::on_examRoomButton_clicked() {
    emit showExamRoomManagement();
}

void Home::on_logoutButton_clicked() {
    UserData::instance().reset();
    emit logout();
}