#include "examlist.h"
#include "ui_examlist.h"
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
#include <QScrollArea>

ExamList::ExamList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ExamList),
    tcpSocket(new QTcpSocket(this))
{
    ui->setupUi(this);
    
    connect(ui->btnBack, &QPushButton::clicked, this, &ExamList::backToClassList);
    connect(ui->btnRefresh, &QPushButton::clicked, this, &ExamList::loadExams);
}

ExamList::~ExamList()
{
    delete ui;
}

void ExamList::loadExams()
{
    tcpSocket->connectToHost(IPADDRESS, PORT);
    if (tcpSocket->waitForConnected(3000)) {
        QJsonObject json;
        json["user_id"] = UserData::instance().getUserId();
        
        QString request = QString("CONTROL GET_MY_EXAM_HISTORY\n%1").arg(QString(QJsonDocument(json).toJson(QJsonDocument::Compact)));
        tcpSocket->write(request.toUtf8());
        tcpSocket->flush();
        
        connect(tcpSocket, &QTcpSocket::readyRead, this, &ExamList::onReadyRead);
    } else {
        QMessageBox::critical(this, "Lỗi", "Không thể kết nối server!");
    }
}

void ExamList::onReadyRead()
{
    QByteArray response = tcpSocket->readAll();
    QString responseStr(response);
    qDebug() << "ExamList response:" << responseStr;
    
    tcpSocket->disconnect(SIGNAL(readyRead()));
    tcpSocket->close();
    
    if (responseStr.contains("EXAM_HISTORY")) {
        int jsonStart = responseStr.indexOf('{');
        if (jsonStart != -1) {
            QJsonDocument doc = QJsonDocument::fromJson(responseStr.mid(jsonStart).toUtf8());
            QJsonObject obj = doc.object();
            exams = obj["data"].toArray();
            displayExams();
        }
    }
}

void ExamList::displayExams()
{
    // Clear existing content
    QLayoutItem *child;
    while ((child = ui->examListLayout->takeAt(0)) != nullptr) {
        if (child->widget()) delete child->widget();
        delete child;
    }
    
    if (exams.isEmpty()) {
        QLabel *emptyLabel = new QLabel("Bạn chưa tham gia bài thi nào.");
        emptyLabel->setAlignment(Qt::AlignCenter);
        emptyLabel->setStyleSheet("color: gray; font-size: 14px; padding: 20px;");
        ui->examListLayout->addWidget(emptyLabel);
        return;
    }
    
    for (int i = 0; i < exams.size(); i++) {
        QJsonObject exam = exams[i].toObject();
        
        QFrame *card = new QFrame();
        card->setFrameShape(QFrame::StyledPanel);
        card->setStyleSheet("QFrame { background-color: white; border: 1px solid #ddd; border-radius: 8px; padding: 10px; margin: 5px; }");
        
        QVBoxLayout *cardLayout = new QVBoxLayout(card);
        
        // Exam name
        QLabel *nameLabel = new QLabel(exam["exam_name"].toString());
        nameLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #333;");
        cardLayout->addWidget(nameLabel);
        
        // Class name
        QLabel *classLabel = new QLabel("Lớp: " + exam["class_name"].toString());
        classLabel->setStyleSheet("color: #666;");
        cardLayout->addWidget(classLabel);
        
        // Score and status
        QHBoxLayout *infoLayout = new QHBoxLayout();
        
        QString status = exam["status"].toString();
        double score = exam["score"].toDouble();
        int correct = exam["correct_answers"].toInt();
        int total = exam["total_questions"].toInt();
        
        QLabel *scoreLabel = new QLabel(QString("Điểm: %1 (%2/%3 câu đúng)")
            .arg(score, 0, 'f', 1).arg(correct).arg(total));
        scoreLabel->setStyleSheet("font-weight: bold; color: #4CAF50;");
        infoLayout->addWidget(scoreLabel);
        
        QLabel *statusLabel = new QLabel(status == "submitted" ? "✓ Đã nộp" : "⏳ Đang làm");
        statusLabel->setStyleSheet(status == "submitted" ? "color: green;" : "color: orange;");
        infoLayout->addWidget(statusLabel);
        
        infoLayout->addStretch();
        cardLayout->addLayout(infoLayout);
        
        // View result button
        int submissionId = exam["submission_id"].toInt();
        QPushButton *viewBtn = new QPushButton("Xem chi tiết");
        viewBtn->setStyleSheet("background-color: #2196F3; color: white; padding: 8px 16px; border-radius: 4px;");
        connect(viewBtn, &QPushButton::clicked, [this, submissionId]() {
            emit viewExamResult(submissionId);
        });
        cardLayout->addWidget(viewBtn);
        
        ui->examListLayout->addWidget(card);
    }
    
    ui->examListLayout->addStretch();
}

void ExamList::onExamClicked(int examId)
{
    // Find exam info and emit signal
    for (int i = 0; i < exams.size(); i++) {
        QJsonObject exam = exams[i].toObject();
        if (exam["exam_id"].toInt() == examId) {
            emit startExam(examId, exam["exam_name"].toString(), exam["time_limit"].toInt());
            break;
        }
    }
}

