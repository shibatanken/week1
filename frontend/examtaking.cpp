#include "examtaking.h"
#include "ui_examtaking.h"
#include "config.h"
#include "userdata.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QMessageBox>
#include <QRadioButton>
#include <QVBoxLayout>

ExamTaking::ExamTaking(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ExamTaking),
    tcpSocket(new QTcpSocket(this)),
    examTimer(new QTimer(this)),
    examId(-1),
    submissionId(-1),
    timeLimit(60),
    remainingSeconds(0),
    currentQuestionIndex(0)
{
    ui->setupUi(this);
    
    connect(examTimer, &QTimer::timeout, this, &ExamTaking::updateTimer);
    connect(ui->btnPrevious, &QPushButton::clicked, this, &ExamTaking::onPreviousClicked);
    connect(ui->btnNext, &QPushButton::clicked, this, &ExamTaking::onNextClicked);
    connect(ui->btnSubmit, &QPushButton::clicked, this, &ExamTaking::onSubmitClicked);
    connect(ui->btnBack, &QPushButton::clicked, this, &ExamTaking::backToExamList);
    
    // Connect radio buttons
    connect(ui->radioA, &QRadioButton::clicked, [this]() { onAnswerSelected(currentQuestionIndex, "A"); });
    connect(ui->radioB, &QRadioButton::clicked, [this]() { onAnswerSelected(currentQuestionIndex, "B"); });
    connect(ui->radioC, &QRadioButton::clicked, [this]() { onAnswerSelected(currentQuestionIndex, "C"); });
    connect(ui->radioD, &QRadioButton::clicked, [this]() { onAnswerSelected(currentQuestionIndex, "D"); });
}

ExamTaking::~ExamTaking()
{
    examTimer->stop();
    delete ui;
}

void ExamTaking::setExamInfo(int examId, QString examName, int timeLimit)
{
    this->examId = examId;
    this->examName = examName;
    this->timeLimit = timeLimit;
    this->remainingSeconds = timeLimit * 60;
    
    ui->lblExamName->setText(examName);
    ui->lblTimer->setText(QString("%1:%2").arg(timeLimit).arg(0, 2, 10, QChar('0')));
    
    // Reset state
    questions = QJsonArray();
    userAnswers.clear();
    currentQuestionIndex = 0;
    
    ui->stackedWidget->setCurrentIndex(0); // Show exam widget
}

void ExamTaking::startExam()
{
    // Join exam first
    tcpSocket->connectToHost(IPADDRESS, PORT);
    if (tcpSocket->waitForConnected(3000)) {
        QJsonObject json;
        json["exam_id"] = examId;
        json["user_id"] = UserData::instance().getUserId();
        
        QString request = QString("CONTROL JOIN_EXAM\n%1").arg(QString(QJsonDocument(json).toJson(QJsonDocument::Compact)));
        tcpSocket->write(request.toUtf8());
        tcpSocket->flush();
        
        connect(tcpSocket, &QTcpSocket::readyRead, this, &ExamTaking::onReadyRead);
    } else {
        QMessageBox::critical(this, "Lỗi", "Không thể kết nối server!");
    }
}

void ExamTaking::onReadyRead()
{
    QByteArray response = tcpSocket->readAll();
    QString responseStr(response);
    qDebug() << "ExamTaking response:" << responseStr;
    
    tcpSocket->disconnect(SIGNAL(readyRead()));
    tcpSocket->close();
    
    if (responseStr.contains("JOIN_EXAM_SUCCESS")) {
        int jsonStart = responseStr.indexOf('{');
        if (jsonStart != -1) {
            QJsonDocument doc = QJsonDocument::fromJson(responseStr.mid(jsonStart).toUtf8());
            QJsonObject obj = doc.object();
            submissionId = obj["submission_id"].toInt();
        }
        loadQuestions();
    }
    else if (responseStr.contains("EXAM_FOR_STUDENT")) {
        int jsonStart = responseStr.indexOf('{');
        if (jsonStart != -1) {
            QJsonDocument doc = QJsonDocument::fromJson(responseStr.mid(jsonStart).toUtf8());
            QJsonObject obj = doc.object();
            questions = obj["questions"].toArray();
            
            if (questions.isEmpty()) {
                QMessageBox::warning(this, "Thông báo", "Bài thi không có câu hỏi!");
                return;
            }
            
            ui->lblTotalQuestions->setText(QString("Tổng: %1 câu").arg(questions.size()));
            displayQuestion(0);
            
            // Start timer
            examTimer->start(1000);
        }
    }
    else if (responseStr.contains("SUBMIT_ANSWER_SUCCESS")) {
        qDebug() << "Answer saved successfully";
    }
    else if (responseStr.contains("SUBMIT_EXAM_SUCCESS")) {
        examTimer->stop();
        int jsonStart = responseStr.indexOf('{');
        if (jsonStart != -1) {
            QJsonDocument doc = QJsonDocument::fromJson(responseStr.mid(jsonStart).toUtf8());
            showResult(doc.object());
        }
    }
}

void ExamTaking::loadQuestions()
{
    tcpSocket->connectToHost(IPADDRESS, PORT);
    if (tcpSocket->waitForConnected(3000)) {
        QJsonObject json;
        json["exam_id"] = examId;
        json["user_id"] = UserData::instance().getUserId();
        
        QString request = QString("CONTROL GET_EXAM_FOR_STUDENT\n%1").arg(QString(QJsonDocument(json).toJson(QJsonDocument::Compact)));
        tcpSocket->write(request.toUtf8());
        tcpSocket->flush();
        
        connect(tcpSocket, &QTcpSocket::readyRead, this, &ExamTaking::onReadyRead);
    }
}

void ExamTaking::displayQuestion(int index)
{
    if (index < 0 || index >= questions.size()) return;
    
    currentQuestionIndex = index;
    QJsonObject q = questions[index].toObject();
    
    int questionId = q["id"].toInt();
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
    
    // Restore previous answer if any
    if (userAnswers.contains(questionId)) {
        QString answer = userAnswers[questionId];
        if (answer == "A") ui->radioA->setChecked(true);
        else if (answer == "B") ui->radioB->setChecked(true);
        else if (answer == "C") ui->radioC->setChecked(true);
        else if (answer == "D") ui->radioD->setChecked(true);
    }
    
    // Update navigation buttons
    ui->btnPrevious->setEnabled(index > 0);
    ui->btnNext->setEnabled(index < questions.size() - 1);
}

void ExamTaking::onAnswerSelected(int questionIndex, QString answer)
{
    if (questionIndex < 0 || questionIndex >= questions.size()) return;
    
    QJsonObject q = questions[questionIndex].toObject();
    int questionId = q["id"].toInt();
    userAnswers[questionId] = answer;
    
    // Send answer to server
    saveCurrentAnswer();
}

void ExamTaking::saveCurrentAnswer()
{
    if (currentQuestionIndex < 0 || currentQuestionIndex >= questions.size()) return;
    
    QJsonObject q = questions[currentQuestionIndex].toObject();
    int questionId = q["id"].toInt();
    
    if (!userAnswers.contains(questionId)) return;
    
    tcpSocket->connectToHost(IPADDRESS, PORT);
    if (tcpSocket->waitForConnected(3000)) {
        QJsonObject json;
        json["submission_id"] = submissionId;
        json["question_id"] = questionId;
        json["answer"] = userAnswers[questionId];
        
        QString request = QString("CONTROL SUBMIT_ANSWER\n%1").arg(QString(QJsonDocument(json).toJson(QJsonDocument::Compact)));
        tcpSocket->write(request.toUtf8());
        tcpSocket->flush();
        
        connect(tcpSocket, &QTcpSocket::readyRead, this, &ExamTaking::onReadyRead);
    }
}

void ExamTaking::updateTimer()
{
    remainingSeconds--;
    
    if (remainingSeconds <= 0) {
        examTimer->stop();
        QMessageBox::information(this, "Hết giờ", "Thời gian làm bài đã hết. Bài thi sẽ được nộp tự động.");
        submitExam();
        return;
    }
    
    int minutes = remainingSeconds / 60;
    int seconds = remainingSeconds % 60;
    ui->lblTimer->setText(QString("%1:%2").arg(minutes).arg(seconds, 2, 10, QChar('0')));
    
    // Warning when 5 minutes left
    if (remainingSeconds == 300) {
        ui->lblTimer->setStyleSheet("color: orange; font-weight: bold; font-size: 18px;");
    }
    // Critical when 1 minute left
    if (remainingSeconds == 60) {
        ui->lblTimer->setStyleSheet("color: red; font-weight: bold; font-size: 18px;");
    }
}

void ExamTaking::onPreviousClicked()
{
    if (currentQuestionIndex > 0) {
        displayQuestion(currentQuestionIndex - 1);
    }
}

void ExamTaking::onNextClicked()
{
    if (currentQuestionIndex < questions.size() - 1) {
        displayQuestion(currentQuestionIndex + 1);
    }
}

void ExamTaking::onSubmitClicked()
{
    int answered = userAnswers.size();
    int total = questions.size();
    
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Xác nhận nộp bài",
        QString("Bạn đã trả lời %1/%2 câu hỏi.\nBạn có chắc muốn nộp bài?").arg(answered).arg(total),
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        submitExam();
    }
}

void ExamTaking::submitExam()
{
    examTimer->stop();
    
    tcpSocket->connectToHost(IPADDRESS, PORT);
    if (tcpSocket->waitForConnected(3000)) {
        QJsonObject json;
        json["submission_id"] = submissionId;
        
        QString request = QString("CONTROL SUBMIT_EXAM\n%1").arg(QString(QJsonDocument(json).toJson(QJsonDocument::Compact)));
        tcpSocket->write(request.toUtf8());
        tcpSocket->flush();
        
        connect(tcpSocket, &QTcpSocket::readyRead, this, &ExamTaking::onReadyRead);
    }
}

void ExamTaking::showResult(const QJsonObject &result)
{
    double score = result["score"].toDouble();
    int total = result["total_questions"].toInt();
    int correct = result["correct_answers"].toInt();
    
    ui->stackedWidget->setCurrentIndex(1); // Show result widget
    ui->lblResultScore->setText(QString::number(score, 'f', 1) + " điểm");
    ui->lblResultDetail->setText(QString("Số câu đúng: %1/%2").arg(correct).arg(total));
    
    // Show detailed answers if available
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

