#include "home.h"
#include "ui_home.h"

Home::Home(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Home)
{
    ui->setupUi(this);
    
    connect(ui->logoutButton, &QPushButton::clicked, this, &Home::on_logoutButton_clicked);
}

Home::~Home() {
    delete ui;
}

void Home::updateUserInfo() {
    QJsonObject userData = UserData::instance().getUserData();
    QString userName = userData.contains("name") ? userData["name"].toString() : "";
    QString email = userData.contains("email") ? userData["email"].toString() : "";
    
    ui->welcomeLabel->setText("Chào mừng, " + userName + "!");
    ui->emailLabel->setText("Email: " + email);
}

void Home::on_classListButton_clicked() {
    emit showClassList();
}

void Home::on_logoutButton_clicked() {
    UserData::instance().reset();
    emit logout();
}