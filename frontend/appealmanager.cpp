#include "appealmanager.h"
#include "ui_appealmanager.h"
#include "config.h"
#include "userdata.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QFrame>
#include <QMessageBox>
#include <QTextEdit>
#include <QInputDialog>

AppealManager::AppealManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AppealManager),
    tcpSocket(new QTcpSocket(this)),
    isTeacherMode(false)
{
    ui->setupUi(this);
    
    connect(ui->btnBack, &QPushButton::clicked, this, &AppealManager::backPressed);
    connect(ui->btnRefresh, &QPushButton::clicked, this, &AppealManager::loadAppeals);
}

AppealManager::~AppealManager()
{
    delete ui;
}

void AppealManager::setMode(bool isTeacher)
{
    isTeacherMode = isTeacher;
    ui->lblTitle->setText(isTeacher ? "📋 Quản lý khiếu nại" : "📋 Khiếu nại của tôi");
}

void AppealManager::loadAppeals()
{
    tcpSocket->connectToHost(IPADDRESS, PORT);
    if (tcpSocket->waitForConnected(3000)) {
        QJsonObject json;
        
        QString command;
        if (isTeacherMode) {
            json["teacher_id"] = UserData::instance().getUserId();
            command = "GET_APPEALS_FOR_TEACHER";
        } else {
            json["user_id"] = UserData::instance().getUserId();
            command = "GET_MY_APPEALS";
        }
        
        QString request = QString("CONTROL %1\n%2").arg(command).arg(QString(QJsonDocument(json).toJson(QJsonDocument::Compact)));
        tcpSocket->write(request.toUtf8());
        tcpSocket->flush();
        
        connect(tcpSocket, &QTcpSocket::readyRead, this, &AppealManager::onReadyRead);
    }
}

void AppealManager::onReadyRead()
{
    QByteArray response = tcpSocket->readAll();
    QString responseStr(response);
    qDebug() << "Appeal response:" << responseStr;
    
    tcpSocket->disconnect(SIGNAL(readyRead()));
    tcpSocket->close();
    
    if (responseStr.contains("APPEALS") || responseStr.contains("MY_APPEALS")) {
        int jsonStart = responseStr.indexOf('{');
        if (jsonStart != -1) {
            QJsonDocument doc = QJsonDocument::fromJson(responseStr.mid(jsonStart).toUtf8());
            appeals = doc.object()["data"].toArray();
            displayAppeals();
        }
    }
    else if (responseStr.contains("REVIEW_APPEAL_SUCCESS")) {
        QMessageBox::information(this, "Thành công", "Đã xử lý khiếu nại!");
        loadAppeals();
    }
    else if (responseStr.contains("SUBMIT_APPEAL_SUCCESS")) {
        QMessageBox::information(this, "Thành công", "Đã gửi khiếu nại!");
        loadAppeals();
    }
}

void AppealManager::displayAppeals()
{
    // Clear existing
    QLayoutItem *child;
    while ((child = ui->appealsLayout->takeAt(0)) != nullptr) {
        if (child->widget()) delete child->widget();
        delete child;
    }
    
    if (appeals.isEmpty()) {
        QLabel *empty = new QLabel("Không có khiếu nại nào.");
        empty->setAlignment(Qt::AlignCenter);
        empty->setStyleSheet("color: gray; padding: 20px;");
        ui->appealsLayout->addWidget(empty);
        return;
    }
    
    for (int i = 0; i < appeals.size(); i++) {
        QJsonObject appeal = appeals[i].toObject();
        
        QFrame *card = new QFrame();
        card->setFrameShape(QFrame::StyledPanel);
        card->setStyleSheet("QFrame { background-color: white; border: 1px solid #ddd; border-radius: 8px; padding: 10px; margin: 5px; }");
        
        QVBoxLayout *cardLayout = new QVBoxLayout(card);
        
        // Exam name
        QLabel *examLabel = new QLabel("Bài thi: " + appeal["exam_name"].toString());
        examLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
        cardLayout->addWidget(examLabel);
        
        // Question
        QLabel *questionLabel = new QLabel("Câu hỏi: " + appeal["question_content"].toString());
        questionLabel->setWordWrap(true);
        cardLayout->addWidget(questionLabel);
        
        // Reason
        QLabel *reasonLabel = new QLabel("Lý do: " + appeal["reason"].toString());
        reasonLabel->setWordWrap(true);
        reasonLabel->setStyleSheet("color: #666;");
        cardLayout->addWidget(reasonLabel);
        
        // Status
        QString status = appeal["status"].toString();
        QString statusColor = status == "pending" ? "orange" : (status == "approved" ? "green" : "red");
        QString statusText = status == "pending" ? "⏳ Đang chờ" : (status == "approved" ? "✓ Đã duyệt" : "✗ Từ chối");
        
        QLabel *statusLabel = new QLabel(statusText);
        statusLabel->setStyleSheet(QString("color: %1; font-weight: bold;").arg(statusColor));
        cardLayout->addWidget(statusLabel);
        
        // Teacher response if any
        if (!appeal["teacher_response"].toString().isEmpty()) {
            QLabel *responseLabel = new QLabel("Phản hồi: " + appeal["teacher_response"].toString());
            responseLabel->setWordWrap(true);
            responseLabel->setStyleSheet("color: #2196F3; font-style: italic;");
            cardLayout->addWidget(responseLabel);
        }
        
        // Teacher actions
        if (isTeacherMode && status == "pending") {
            QHBoxLayout *actionsLayout = new QHBoxLayout();
            
            int appealId = appeal["id"].toInt();
            
            QPushButton *approveBtn = new QPushButton("✓ Duyệt");
            approveBtn->setStyleSheet("background-color: #4CAF50; color: white; padding: 5px 15px;");
            connect(approveBtn, &QPushButton::clicked, [this, appealId]() {
                onReviewAppeal(appealId, "approved");
            });
            actionsLayout->addWidget(approveBtn);
            
            QPushButton *rejectBtn = new QPushButton("✗ Từ chối");
            rejectBtn->setStyleSheet("background-color: #f44336; color: white; padding: 5px 15px;");
            connect(rejectBtn, &QPushButton::clicked, [this, appealId]() {
                onReviewAppeal(appealId, "rejected");
            });
            actionsLayout->addWidget(rejectBtn);
            
            actionsLayout->addStretch();
            cardLayout->addLayout(actionsLayout);
        }
        
        // Student info for teacher
        if (isTeacherMode) {
            QLabel *studentLabel = new QLabel("Sinh viên: " + appeal["student_name"].toString() + " (" + appeal["student_email"].toString() + ")");
            studentLabel->setStyleSheet("color: #999; font-size: 11px;");
            cardLayout->addWidget(studentLabel);
        }
        
        ui->appealsLayout->addWidget(card);
    }
    
    ui->appealsLayout->addStretch();
}

void AppealManager::onSubmitAppeal()
{
    // This would be called from exam result page
}

void AppealManager::onReviewAppeal(int appealId, QString status)
{
    QString response = "";
    bool ok;
    response = QInputDialog::getText(this, "Phản hồi", "Nhập phản hồi (tùy chọn):", QLineEdit::Normal, "", &ok);
    
    if (!ok) return;
    
    tcpSocket->connectToHost(IPADDRESS, PORT);
    if (tcpSocket->waitForConnected(3000)) {
        QJsonObject json;
        json["appeal_id"] = appealId;
        json["status"] = status;
        json["response"] = response;
        json["score_adjustment"] = 0; // Could add input for this
        
        QString request = QString("CONTROL REVIEW_APPEAL\n%1").arg(QString(QJsonDocument(json).toJson(QJsonDocument::Compact)));
        tcpSocket->write(request.toUtf8());
        tcpSocket->flush();
        
        connect(tcpSocket, &QTcpSocket::readyRead, this, &AppealManager::onReadyRead);
    }
}

