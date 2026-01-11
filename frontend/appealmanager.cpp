#include "appealmanager.h"
#include "ui_appealmanager.h"
#include "config.h"
#include "userdata.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QFrame>
#include <QMessageBox>
#include <QTextEdit>
#include <QInputDialog>
#include <QComboBox>
#include <QDebug>

AppealManager::AppealManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AppealManager),
    tcpSocket(new QTcpSocket(this)),
    isTeacherMode(false),
    currentSubmissionId(-1)
{
    ui->setupUi(this);
    
    connect(ui->btnBack, &QPushButton::clicked, this, &AppealManager::backPressed);
    connect(ui->btnRefresh, &QPushButton::clicked, this, &AppealManager::loadAppeals);
}

void AppealManager::setSubmissionId(int submissionId)
{
    currentSubmissionId = submissionId;
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
    // Disconnect any existing connection
    if (tcpSocket->state() != QAbstractSocket::UnconnectedState) {
        tcpSocket->disconnect(this);
        tcpSocket->close();
    }
    
    tcpSocket->connectToHost(IPADDRESS, PORT);
    if (tcpSocket->waitForConnected(3000)) {
        QJsonObject json;
        
        QString command;
        if (isTeacherMode) {
            json["teacher_id"] = UserData::instance().getUserId();
            command = "GET_APPEALS_FOR_TEACHER";
            qDebug() << "Loading appeals for teacher, teacher_id:" << UserData::instance().getUserId();
        } else {
            json["user_id"] = UserData::instance().getUserId();
            command = "GET_MY_APPEALS";
            qDebug() << "Loading appeals for student, user_id:" << UserData::instance().getUserId();
        }
        
        QString request = QString("CONTROL %1\n%2").arg(command).arg(QString(QJsonDocument(json).toJson(QJsonDocument::Compact)));
        qDebug() << "Sending request:" << request;
        tcpSocket->write(request.toUtf8());
        tcpSocket->flush();
        
        // Disconnect any existing connection first
        tcpSocket->disconnect(this);
        connect(tcpSocket, &QTcpSocket::readyRead, this, &AppealManager::onReadyRead);
    } else {
        qDebug() << "Failed to connect to server for appeals";
    }
}

void AppealManager::onReadyRead()
{
    QByteArray response = tcpSocket->readAll();
    QString responseStr(response);
    qDebug() << "Appeal response:" << responseStr;
    
    tcpSocket->disconnect(this);
    tcpSocket->close();
    
    qDebug() << "AppealManager response:" << responseStr;
    
    if (responseStr.contains("APPEALS") || responseStr.contains("MY_APPEALS")) {
        int jsonStart = responseStr.indexOf('{');
        if (jsonStart != -1) {
            QString jsonStr = responseStr.mid(jsonStart);
            QJsonParseError error;
            QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8(), &error);
            
            if (error.error != QJsonParseError::NoError) {
                qDebug() << "JSON parse error:" << error.errorString() << "at offset" << error.offset;
                qDebug() << "JSON string:" << jsonStr;
                return;
            }
            
            QJsonObject obj = doc.object();
            qDebug() << "Parsed object keys:" << obj.keys();
            
            if (obj.contains("data")) {
                appeals = obj["data"].toArray();
                qDebug() << "Found" << appeals.size() << "appeals";
                displayAppeals();
            } else {
                qDebug() << "No 'data' key in response!";
            }
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
        
        // Teacher comment if any
        if (!appeal["teacher_comment"].toString().isEmpty()) {
            QLabel *commentLabel = new QLabel("Bình luận: " + appeal["teacher_comment"].toString());
            commentLabel->setWordWrap(true);
            commentLabel->setStyleSheet("color: #FF9800; font-style: italic; padding: 5px; background-color: #FFF3E0; border-radius: 4px;");
            cardLayout->addWidget(commentLabel);
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
    // This is called from dynamically created form
    // The actual submission is handled in displayAppealForm via lambda
}

void AppealManager::displayAppealForm()
{
    // Clear existing
    QLayoutItem *child;
    while ((child = ui->appealsLayout->takeAt(0)) != nullptr) {
        if (child->widget()) delete child->widget();
        delete child;
    }
    
    QLabel *formTitle = new QLabel("📝 Gửi khiếu nại mới");
    formTitle->setStyleSheet("font-size: 16px; font-weight: bold; margin-bottom: 10px;");
    ui->appealsLayout->addWidget(formTitle);
    
    QLabel *questionLabel = new QLabel("Chọn câu hỏi cần khiếu nại:");
    ui->appealsLayout->addWidget(questionLabel);
    
    QComboBox *cmbQuestion = new QComboBox();
    cmbQuestion->setObjectName("cmbQuestion");
    // TODO: Populate with questions from submission
    ui->appealsLayout->addWidget(cmbQuestion);
    
    QLabel *reasonLabel = new QLabel("Lý do khiếu nại:");
    ui->appealsLayout->addWidget(reasonLabel);
    
    QTextEdit *txtReason = new QTextEdit();
    txtReason->setObjectName("txtReason");
    txtReason->setPlaceholderText("Nhập lý do khiếu nại của bạn...");
    txtReason->setMaximumHeight(100);
    ui->appealsLayout->addWidget(txtReason);
    
    QPushButton *submitBtn = new QPushButton("📤 Gửi khiếu nại");
    submitBtn->setStyleSheet("background-color: #ff9800; color: white; font-weight: bold; padding: 10px 20px;");
    connect(submitBtn, &QPushButton::clicked, [this, cmbQuestion, txtReason]() {
        QString reason = txtReason->toPlainText().trimmed();
        int questionId = cmbQuestion->currentData().toInt();
        
        if (reason.isEmpty()) {
            QMessageBox::warning(this, "Lỗi", "Vui lòng nhập lý do khiếu nại!");
            return;
        }
        
        tcpSocket->connectToHost(IPADDRESS, PORT);
        if (tcpSocket->waitForConnected(3000)) {
            QJsonObject json;
            json["submission_id"] = currentSubmissionId;
            json["question_id"] = questionId;
            json["user_id"] = UserData::instance().getUserId();
            json["reason"] = reason;
            
            QString request = QString("CONTROL SUBMIT_APPEAL\n%1").arg(QString(QJsonDocument(json).toJson(QJsonDocument::Compact)));
            tcpSocket->write(request.toUtf8());
            tcpSocket->flush();
            
            connect(tcpSocket, &QTcpSocket::readyRead, this, &AppealManager::onReadyRead);
        }
    });
    ui->appealsLayout->addWidget(submitBtn);
    
    ui->appealsLayout->addStretch();
}

void AppealManager::onReviewAppeal(int appealId, QString status)
{
    // Create dialog for review
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle(status == "approved" ? "Duyệt khiếu nại" : "Từ chối khiếu nại");
    dialog->setMinimumSize(400, 300);
    
    QVBoxLayout *layout = new QVBoxLayout(dialog);
    
    // Teacher comment field (optional)
    QLabel *commentLabel = new QLabel("Phản hồi cho sinh viên (tùy chọn):");
    layout->addWidget(commentLabel);
    
    QTextEdit *commentEdit = new QTextEdit();
    commentEdit->setPlaceholderText("Nhập phản hồi cho sinh viên...");
    commentEdit->setMaximumHeight(100);
    layout->addWidget(commentEdit);
    
    // Score adjustment (optional)
    QLabel *scoreLabel = new QLabel("Điều chỉnh điểm (nếu duyệt, tùy chọn):");
    layout->addWidget(scoreLabel);
    
    QLineEdit *scoreEdit = new QLineEdit();
    scoreEdit->setPlaceholderText("0.0");
    scoreEdit->setValidator(new QDoubleValidator(-100, 100, 2, this));
    layout->addWidget(scoreEdit);
    
    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *cancelBtn = new QPushButton("Hủy");
    QPushButton *confirmBtn = new QPushButton(status == "approved" ? "✓ Duyệt" : "✗ Từ chối");
    confirmBtn->setStyleSheet(status == "approved" ? 
        "background-color: #4CAF50; color: white;" : 
        "background-color: #f44336; color: white;");
    
    connect(cancelBtn, &QPushButton::clicked, dialog, &QDialog::reject);
    connect(confirmBtn, &QPushButton::clicked, dialog, &QDialog::accept);
    
    buttonLayout->addStretch();
    buttonLayout->addWidget(cancelBtn);
    buttonLayout->addWidget(confirmBtn);
    layout->addLayout(buttonLayout);
    
    if (dialog->exec() == QDialog::Accepted) {
        QString teacherComment = commentEdit->toPlainText().trimmed();
        double scoreAdj = scoreEdit->text().toDouble();
        
        tcpSocket->connectToHost(IPADDRESS, PORT);
        if (tcpSocket->waitForConnected(3000)) {
            QJsonObject json;
            json["appeal_id"] = appealId;
            json["status"] = status;
            json["response"] = ""; // Keep for backward compatibility
            json["teacher_comment"] = teacherComment;
            json["score_adjustment"] = scoreAdj;
            
            QString request = QString("CONTROL REVIEW_APPEAL\n%1").arg(QString(QJsonDocument(json).toJson(QJsonDocument::Compact)));
            tcpSocket->write(request.toUtf8());
            tcpSocket->flush();
            
            connect(tcpSocket, &QTcpSocket::readyRead, this, &AppealManager::onReadyRead);
        }
    }
    
    dialog->deleteLater();
}

