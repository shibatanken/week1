#include "signin.h"
#include "ui_signin.h"
#include "config.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QDateTime>
#include <QDebug>
#include <QMessageBox>
#include <QTimer>

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

    tcpSocket->abort(); // Close previous connection if any
    tcpSocket->connectToHost(IPADDRESS, 8081);
    
    // Change wait time to 3000ms (3s) to be faster/more responsive
    if (tcpSocket->waitForConnected(3000)) {
        tcpSocket->write(dataString.toUtf8());
        tcpSocket->flush();
    } else {
        QMessageBox::critical(this, "Lỗi kết nối", 
            QString("Không thể kết nối đến server (%1:8081).\nLỗi: %2\n\nHãy chắc chắn rằng Backend Server đang chạy.")
            .arg(IPADDRESS).arg(tcpSocket->errorString()));
    }
}

void Signin::onReadyRead() {
    QByteArray response = tcpSocket->readAll();
    QString responseString(response);

    if (responseString.startsWith("NOTIFICATION LOGIN_FAILURE")) {
        // Parse reason if available? Usually just LOGIN_FAILURE
        ui->responseLabel->setText("Đăng nhập thất bại. Kiểm tra email/mật khẩu.");
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
                
                // First navigate to main screen
                emit loginSuccess();

                // THEN show popup after delay so main screen loads first
                // Check for unread appeals
                int unreadCount = jsonObj["unread_appeals_count"].toInt();
                if (unreadCount > 0) {
                    QString role = UserData::instance().getRole();
                    QString message;
                    if (role == "teacher") {
                        message = QString("Bạn có %1 khiếu nại mới cần xử lý!").arg(unreadCount);
                    } else {
                        message = QString("Bạn có %1 thông báo khiếu nại mới!").arg(unreadCount);
                    }
                    

                    // Delay 500ms for main screen to load
                    QTimer::singleShot(500, this, [this, message]() {
                        QMessageBox msgBox;
                        msgBox.setWindowTitle("🔔 Thông báo khiếu nại");
                        msgBox.setText(message);
                        msgBox.setInformativeText("Bạn có muốn xem ngay không?");
                        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                        msgBox.setDefaultButton(QMessageBox::Yes);
                        msgBox.setIcon(QMessageBox::Information);
                    
                        int ret = msgBox.exec();
                        if (ret == QMessageBox::Yes) {
                            emit showAppealManager();
                        }
                    });
                }
            }
        }
    } else {
        ui->responseLabel->setText("Phản hồi không xác định từ server");
        ui->responseLabel->setStyleSheet("QLabel { color : orange; }");
    }
}

void Signin::onConnected() {
    qDebug() << "Connected to server.";
}

void Signin::onDisconnected() {
    qDebug() << "Disconnected from server.";
}
