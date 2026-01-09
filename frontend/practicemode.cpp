#include "practicemode.h"
#include "ui_practicemode.h"
#include "config.h"
#include "userdata.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>

PracticeMode::PracticeMode(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PracticeMode),
    tcpSocket(new QTcpSocket(this)),
    classId(-1),
    sessionId(-1),
    currentQuestionIndex(0)
{
    ui->setupUi(this);
    
    connect(ui->btnBack, &QPushButton::clicked, this, &PracticeMode::backToClassDetail);
    connect(ui->btnStart, &QPushButton::clicked, this, &PracticeMode::onStartPractice);
    connect(ui->btnNext, &QPushButton::clicked, this, &PracticeMode::onNextQuestion);
    connect(ui->btnFinish, &QPushButton::clicked, this, &PracticeMode::onFinishPractice);
    
    connect(ui->radioA, &QRadioButton::clicked, [this]() { onAnswerSelected("A"); });
    connect(ui->radioB, &QRadioButton::clicked, [this]() { onAnswerSelected("B"); });
    connect(ui->radioC, &QRadioButton::clicked, [this]() { onAnswerSelected("C"); });
    connect(ui->radioD, &QRadioButton::clicked, [this]() { onAnswerSelected("D"); });
}

PracticeMode::~PracticeMode()
{
    delete ui;
}

void PracticeMode::setClassInfo(int classId, QString className)
{
    this->classId = classId;
    this->className = className;
    ui->lblClassName->setText("Luyện tập: " + className);
    ui->stackedWidget->setCurrentIndex(0); // Show start page
}

void PracticeMode::onStartPractice()
{
    int numQuestions = ui->spinNumQuestions->value();
    
    tcpSocket->connectToHost(IPADDRESS, PORT);
    if (tcpSocket->waitForConnected(3000)) {
        QJsonObject json;
        json["user_id"] = UserData::instance().getUserId();
        json["class_id"] = classId;
        json["num_questions"] = numQuestions;
        
        QString request = QString("CONTROL START_PRACTICE\n%1").arg(QString(QJsonDocument(json).toJson(QJsonDocument::Compact)));
        tcpSocket->write(request.toUtf8());
        tcpSocket->flush();
        
        connect(tcpSocket, &QTcpSocket::readyRead, this, &PracticeMode::onReadyRead);
    }
}

void PracticeMode::onReadyRead()
{
    QByteArray response = tcpSocket->readAll();
    QString responseStr(response);
    qDebug() << "Practice response:" << responseStr;
    
    tcpSocket->disconnect(SIGNAL(readyRead()));
    tcpSocket->close();
    
    if (responseStr.contains("START_PRACTICE_SUCCESS")) {
        int jsonStart = responseStr.indexOf('{');
        if (jsonStart != -1) {
            QJsonDocument doc = QJsonDocument::fromJson(responseStr.mid(jsonStart).toUtf8());
            sessionId = doc.object()["session_id"].toInt();
            loadPracticeQuestions();
        }
    }
    else if (responseStr.contains("PRACTICE_QUESTIONS")) {
        int jsonStart = responseStr.indexOf('{');
        if (jsonStart != -1) {
            QJsonDocument doc = QJsonDocument::fromJson(responseStr.mid(jsonStart).toUtf8());
            questions = doc.object()["data"].toArray();
            
            if (questions.isEmpty()) {
                QMessageBox::warning(this, "Thông báo", "Không có câu hỏi trong ngân hàng!");
                return;
            }
            
            currentQuestionIndex = 0;
            ui->stackedWidget->setCurrentIndex(1); // Show practice page
            displayQuestion(0);
        }
    }
    else if (responseStr.contains("SUBMIT_PRACTICE_ANSWER_SUCCESS")) {
        // Show correct/incorrect feedback
        QJsonObject q = questions[currentQuestionIndex].toObject();
        QString correct = q["correct_option"].toString();
        // Could show feedback here
    }
    else if (responseStr.contains("FINISH_PRACTICE_SUCCESS")) {
        int jsonStart = responseStr.indexOf('{');
        if (jsonStart != -1) {
            QJsonDocument doc = QJsonDocument::fromJson(responseStr.mid(jsonStart).toUtf8());
            showResult(doc.object());
        }
    }
}

void PracticeMode::loadPracticeQuestions()
{
    tcpSocket->connectToHost(IPADDRESS, PORT);
    if (tcpSocket->waitForConnected(3000)) {
        QJsonObject json;
        json["session_id"] = sessionId;
        
        QString request = QString("CONTROL GET_PRACTICE_QUESTIONS\n%1").arg(QString(QJsonDocument(json).toJson(QJsonDocument::Compact)));
        tcpSocket->write(request.toUtf8());
        tcpSocket->flush();
        
        connect(tcpSocket, &QTcpSocket::readyRead, this, &PracticeMode::onReadyRead);
    }
}

void PracticeMode::displayQuestion(int index)
{
    if (index < 0 || index >= questions.size()) return;
    
    currentQuestionIndex = index;
    QJsonObject q = questions[index].toObject();
    
    ui->lblQuestionNumber->setText(QString("Câu %1/%2").arg(index + 1).arg(questions.size()));
    ui->lblQuestionContent->setText(q["content"].toString());
    ui->radioA->setText("A. " + q["option_a"].toString());
    ui->radioB->setText("B. " + q["option_b"].toString());
    ui->radioC->setText("C. " + q["option_c"].toString());
    ui->radioD->setText("D. " + q["option_d"].toString());
    
    // Clear selection
    ui->radioA->setAutoExclusive(false);
    ui->radioB->setAutoExclusive(false);
    ui->radioC->setAutoExclusive(false);
    ui->radioD->setAutoExclusive(false);
    ui->radioA->setChecked(false);
    ui->radioB->setChecked(false);
    ui->radioC->setChecked(false);
    ui->radioD->setChecked(false);
    ui->radioA->setAutoExclusive(true);
    ui->radioB->setAutoExclusive(true);
    ui->radioC->setAutoExclusive(true);
    ui->radioD->setAutoExclusive(true);
    
    // Restore previous answer
    QString prevAnswer = q["user_answer"].toString();
    if (prevAnswer == "A") ui->radioA->setChecked(true);
    else if (prevAnswer == "B") ui->radioB->setChecked(true);
    else if (prevAnswer == "C") ui->radioC->setChecked(true);
    else if (prevAnswer == "D") ui->radioD->setChecked(true);
    
    ui->btnNext->setText(index < questions.size() - 1 ? "Câu tiếp ▶" : "Hoàn thành");
}

void PracticeMode::onAnswerSelected(QString answer)
{
    QJsonObject q = questions[currentQuestionIndex].toObject();
    int questionId = q["id"].toInt();
    
    tcpSocket->connectToHost(IPADDRESS, PORT);
    if (tcpSocket->waitForConnected(3000)) {
        QJsonObject json;
        json["session_id"] = sessionId;
        json["question_id"] = questionId;
        json["answer"] = answer;
        
        QString request = QString("CONTROL SUBMIT_PRACTICE_ANSWER\n%1").arg(QString(QJsonDocument(json).toJson(QJsonDocument::Compact)));
        tcpSocket->write(request.toUtf8());
        tcpSocket->flush();
        
        connect(tcpSocket, &QTcpSocket::readyRead, this, &PracticeMode::onReadyRead);
    }
}

void PracticeMode::onNextQuestion()
{
    if (currentQuestionIndex < questions.size() - 1) {
        displayQuestion(currentQuestionIndex + 1);
    } else {
        onFinishPractice();
    }
}

void PracticeMode::onFinishPractice()
{
    tcpSocket->connectToHost(IPADDRESS, PORT);
    if (tcpSocket->waitForConnected(3000)) {
        QJsonObject json;
        json["session_id"] = sessionId;
        
        QString request = QString("CONTROL FINISH_PRACTICE\n%1").arg(QString(QJsonDocument(json).toJson(QJsonDocument::Compact)));
        tcpSocket->write(request.toUtf8());
        tcpSocket->flush();
        
        connect(tcpSocket, &QTcpSocket::readyRead, this, &PracticeMode::onReadyRead);
    }
}

void PracticeMode::showResult(const QJsonObject &result)
{
    double score = result["score"].toDouble();
    int correct = result["correct_answers"].toInt();
    int total = result["total_questions"].toInt();
    
    ui->stackedWidget->setCurrentIndex(2); // Show result page
    ui->lblResultScore->setText(QString::number(score, 'f', 1) + " điểm");
    ui->lblResultDetail->setText(QString("Số câu đúng: %1/%2").arg(correct).arg(total));
    
    // Show detailed answers
    if (result.contains("answers")) {
        QJsonArray answers = result["answers"].toArray();
        QString details;
        for (int i = 0; i < answers.size(); i++) {
            QJsonObject a = answers[i].toObject();
            QString status = a["is_correct"].toBool() ? "✓" : "✗";
            details += QString("Câu %1: %2 (Đáp án: %3, Bạn chọn: %4)\n")
                .arg(i + 1)
                .arg(status)
                .arg(a["correct_option"].toString())
                .arg(a["user_answer"].toString());
        }
        ui->textResultDetails->setText(details);
    }
}

