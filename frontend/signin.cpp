#include "signin.h"
#include "ui_signin.h"
#include "config.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QDateTime>
#include <QDebug>
#include <QMessageBox>

Signin::Signin(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Signin)
    , tcpSocket(new QTcpSocket(this))
{
    ui->setupUi(this);
    connect(tcpSocket, &QTcpSocket::readyRead, this, &Signin::onReadyRead);
    connect(tcpSocket, &QTcpSocket::connected, this, &Signin::onConnected);
    connect(tcpSocket, &QTcpSocket::disconnected, this, &Signin::onDisconnected);

    connect(ui->signupButton, &QPushButton::clicked, [this]() {
        emit showSignup();
    });
}

Signin::~Signin() {
    delete ui;
}

void Signin::on_signinButton_clicked() {
    QJsonObject json;
    json["email"] = ui->emailLineEdit->text();
    json["password"] = ui->passwordLineEdit->text();

    QJsonDocument doc(json);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);

    QString dataString = QString("CONTROL LOGIN\n%1").arg(QString(jsonData));

    tcpSocket->connectToHost(IPADDRESS, 8081);
    if (tcpSocket->waitForConnected()) {
        tcpSocket->write(dataString.toUtf8());
        tcpSocket->flush();
    }
}

void Signin::onReadyRead() {
    QByteArray response = tcpSocket->readAll();
    QString responseString(response);

    if (responseString.startsWith("NOTIFICATION LOGIN_FAILURE")) {
        ui->responseLabel->setText("Đăng nhập thất bại");
        ui->responseLabel->setStyleSheet("QLabel { color : red; }");
        qDebug() << "Login Response:" << responseString;
    } else if (responseString.startsWith("NOTIFICATION LOGIN_SUCCESS")) {
        qDebug() << "Login Response:" << responseString;

        int jsonStartIndex = responseString.indexOf('{');
        if (jsonStartIndex != -1) {
            QString jsonString = responseString.mid(jsonStartIndex);
            QJsonParseError parseError;
            QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonString.toUtf8(), &parseError);
            if (parseError.error == QJsonParseError::NoError) {
                QJsonObject jsonObj = jsonDoc.object();
                int userId = jsonObj["user_id"].toInt();
                UserData::instance().setUserData(jsonObj);

                ui->responseLabel->setText("Đăng nhập thành công! User ID: " + QString::number(userId));
                ui->responseLabel->setStyleSheet("QLabel { color : green; }");

                // Reset form
                ui->emailLineEdit->setText("");
                ui->passwordLineEdit->setText("");

                emit loginSuccess();
            }
        }
    } else {
        ui->responseLabel->setText("Unknown response from server");
        ui->responseLabel->setStyleSheet("QLabel { color : orange; }");
    }
}

void Signin::onConnected() {
    qDebug() << "Connected to server.";
}

void Signin::onDisconnected() {
    qDebug() << "Disconnected from server.";
}