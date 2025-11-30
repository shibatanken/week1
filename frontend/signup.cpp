#include "signup.h"
#include "ui_signup.h"
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include "config.h"

Signup::Signup(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Signup)
    , tcpSocket(new QTcpSocket(this))
{
    ui->setupUi(this);

    connect(ui->signinButton, &QPushButton::clicked, [this]() {
        emit showSignin();
    });
}

Signup::~Signup() {
    delete ui;
}

void Signup::on_signupButton_clicked() {
    handleSignup();
}

void Signup::handleSignup() {
    QJsonObject json;
    json["email"] = ui->emailLineEdit->text();
    json["password"] = ui->passwordLineEdit->text();
    json["name"] = ui->usernameLineEdit->text();
    json["dob"] = ui->dateTimeEdit->dateTime().toString(Qt::ISODate);

    QJsonDocument doc(json);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);

    QString dataString = QString("CONTROL SIGN_UP\n%1").arg(QString(jsonData));

    tcpSocket->connectToHost(IPADDRESS, 8081);
    if (tcpSocket->waitForConnected()) {
        tcpSocket->write(dataString.toUtf8());
        tcpSocket->flush();
    }

    connect(tcpSocket, &QTcpSocket::readyRead, this, &Signup::handleSignupResponse);
}

void Signup::handleSignupResponse() {
    QByteArray response = tcpSocket->readAll();
    QString responseString(response);

    if (responseString.startsWith("NOTIFICATION SIGN_UP_SUCCESS")) {
        QMessageBox::information(this, "Thông báo", "Đăng ký tài khoản thành công");
    } else {
        QMessageBox::warning(this, "Thông báo", "Đăng ký tài khoản thất bại");
    }
}