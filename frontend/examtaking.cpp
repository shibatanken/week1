#include "examtaking.h"
#include "ui_examtaking.h"
#include "appealdialog.h"
#include "config.h"
#include "userdata.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonParseError>
#include <QMessageBox>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QDebug>
#include <QtMath>
#include <QEventLoop>
#include <QTimer>
#include <QSharedPointer>
#include <QApplication>
#include <QCloseEvent>

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
    connect(ui->btnBackToList, &QPushButton::clicked, this, &ExamTaking::onBackToListClicked);
    connect(ui->btnAppeal, &QPushButton::clicked, this, &ExamTaking::onAppealClicked);
    
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
    qDebug() << "=== setExamInfo ===";
    qDebug() << "examId:" << examId;
    qDebug() << "examName:" << examName;
    qDebug() << "timeLimit:" << timeLimit;
    
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
    submissionId = -1;
    
    ui->stackedWidget->setCurrentIndex(0); // Show exam widget
}

void ExamTaking::startExam()
{
    // Reset socket state
    if (tcpSocket->state() != QAbstractSocket::UnconnectedState) {
        tcpSocket->disconnect(this);
        tcpSocket->close();
    }
    
    // Join exam first
    tcpSocket->connectToHost(IPADDRESS, PORT);
    if (tcpSocket->waitForConnected(3000)) {
        QJsonObject json;
        json["exam_id"] = examId;
        json["user_id"] = UserData::instance().getUserId();
        
        QString request = QString("CONTROL JOIN_EXAM\n%1").arg(QString(QJsonDocument(json).toJson(QJsonDocument::Compact)));
        qDebug() << "Joining exam_id:" << examId;
        tcpSocket->write(request.toUtf8());
        tcpSocket->flush();
        
        // Disconnect any existing connection first
        tcpSocket->disconnect(this);
        connect(tcpSocket, &QTcpSocket::readyRead, this, &ExamTaking::onReadyRead);
    } else {
        QMessageBox::critical(this, "Lỗi", "Không thể kết nối server!");
        emit backToExamList();
    }
}

void ExamTaking::onReadyRead()
{
    QByteArray response = tcpSocket->readAll();
    QString responseStr(response);
    qDebug() << "=== ExamTaking onReadyRead ===";
    qDebug() << "Full response:" << responseStr;
    qDebug() << "Response length:" << responseStr.length();
    
    // Disconnect and close socket properly
    tcpSocket->disconnect(this);
    tcpSocket->close();
    
    if (responseStr.contains("JOIN_EXAM_SUCCESS")) {
        qDebug() << "Received JOIN_EXAM_SUCCESS";
        int jsonStart = responseStr.indexOf('{');
        if (jsonStart != -1) {
            QString jsonStr = responseStr.mid(jsonStart);
            qDebug() << "JSON string:" << jsonStr;
            QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8());
            if (doc.isNull()) {
                qDebug() << "ERROR: Failed to parse JSON!";
            } else {
                QJsonObject obj = doc.object();
                submissionId = obj["submission_id"].toInt();
                qDebug() << "Got submission_id:" << submissionId;
            }
        } else {
            qDebug() << "ERROR: No JSON found in response!";
        }
        if (submissionId > 0) {
            qDebug() << "Calling loadQuestions() for exam_id:" << examId;
            loadQuestions();
        } else {
            qDebug() << "ERROR: Invalid submission_id:" << submissionId;
            QMessageBox::critical(this, "Lỗi", "Không thể tạo phiên thi!");
            emit backToExamList();
        }
    }
    else if (responseStr.contains("EXAM_FOR_STUDENT")) {
        qDebug() << "Received EXAM_FOR_STUDENT";
        
        // Find JSON start - look for first '{'
        int jsonStart = responseStr.indexOf('{');
        if (jsonStart == -1) {
            qDebug() << "ERROR: No '{' found in response!";
            qDebug() << "Response preview:" << responseStr.left(200);
            QMessageBox::critical(this, "Lỗi", 
                QString("Không tìm thấy dữ liệu JSON trong response!\n\nResponse: %1").arg(responseStr.left(100)));
            emit backToExamList();
            return;
        }
        
        // Extract JSON string and trim
        QString jsonStr = responseStr.mid(jsonStart).trimmed();
        qDebug() << "JSON string (first 500 chars):" << jsonStr.left(500);
        qDebug() << "JSON string length:" << jsonStr.length();
        
        // Try to find the end of JSON object - look for matching closing brace
        // This handles cases where response might have trailing data
        int braceCount = 0;
        int jsonEnd = -1;
        for (int i = 0; i < jsonStr.length(); i++) {
            if (jsonStr[i] == '{') braceCount++;
            if (jsonStr[i] == '}') {
                braceCount--;
                if (braceCount == 0) {
                    jsonEnd = i + 1;
                    break;
                }
            }
        }
        
        if (jsonEnd > 0) {
            jsonStr = jsonStr.left(jsonEnd);
            qDebug() << "Trimmed JSON to:" << jsonStr.length() << "chars";
        }
        
        // Parse with error checking
        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8(), &parseError);
        
        if (parseError.error != QJsonParseError::NoError) {
            qDebug() << "ERROR: JSON Parse Error!";
            qDebug() << "Error code:" << parseError.error;
            qDebug() << "Error message:" << parseError.errorString();
            qDebug() << "Error offset:" << parseError.offset;
            qDebug() << "JSON around error:" << jsonStr.mid(qMax(0, parseError.offset - 50), 100);
            QMessageBox::critical(this, "Lỗi", 
                QString("Không thể parse JSON từ server!\n\nLỗi: %1\nVị trí: %2")
                .arg(parseError.errorString())
                .arg(parseError.offset));
            emit backToExamList();
            return;
        }
        
        if (doc.isNull() || !doc.isObject()) {
            qDebug() << "ERROR: Document is null or not an object!";
            QMessageBox::critical(this, "Lỗi", "Dữ liệu từ server không hợp lệ!");
            emit backToExamList();
            return;
        }
        
        QJsonObject obj = doc.object();
        qDebug() << "Parsed object keys:" << obj.keys();
        
        if (obj.contains("questions")) {
            questions = obj["questions"].toArray();
            qDebug() << "Questions array size:" << questions.size();
            
            if (questions.size() > 0) {
                qDebug() << "First question keys:" << questions[0].toObject().keys();
            }
        } else {
            qDebug() << "ERROR: 'questions' key not found in response!";
            qDebug() << "Available keys:" << obj.keys();
            QMessageBox::warning(this, "Lỗi", 
                QString("Response không có key 'questions'!\n\nKeys có sẵn: %1")
                .arg(obj.keys().join(", ")));
            emit backToExamList();
            return;
        }
        
        if (questions.isEmpty()) {
            qDebug() << "WARNING: Questions array is empty!";
            qDebug() << "exam_id used:" << examId;
            QMessageBox::warning(this, "Thông báo", 
                QString("Bài thi không có câu hỏi!\n\nExam ID: %1\nVui lòng liên hệ giáo viên.").arg(examId));
            emit backToExamList();
            return;
        }
        
        ui->lblTotalQuestions->setText(QString("Tổng: %1 câu").arg(questions.size()));
        displayQuestion(0);
        
        // Start timer
        examTimer->start(1000);
    }
    else if (responseStr.contains("SUBMIT_ANSWER_SUCCESS")) {
        qDebug() << "Answer saved successfully";
    }
    else if (responseStr.contains("SUBMIT_EXAM_SUCCESS")) {
        examTimer->stop();
        int jsonStart = responseStr.indexOf('{');
        if (jsonStart != -1) {
            QString jsonStr = responseStr.mid(jsonStart);
            qDebug() << "=== SUBMIT_EXAM_SUCCESS response ===";
            qDebug() << "Full JSON:" << jsonStr;
            
            QJsonParseError error;
            QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8(), &error);
            if (error.error != QJsonParseError::NoError) {
                qDebug() << "JSON parse error:" << error.errorString() << "at offset" << error.offset;
                return;
            }
            
            QJsonObject resultObj = doc.object();
            qDebug() << "Parsed object keys:" << resultObj.keys();
            
            if (resultObj.contains("answers")) {
                QJsonArray answers = resultObj["answers"].toArray();
                qDebug() << "Answers array size:" << answers.size();
                for (int i = 0; i < answers.size(); i++) {
                    QJsonObject a = answers[i].toObject();
                    qDebug() << "Answer" << (i+1) << "user_answer:" << a["user_answer"].toString() << "is_correct:" << a["is_correct"].toBool();
                }
            }
            
            showResult(resultObj);
        }
    }
    else if (responseStr.contains("JOIN_EXAM_FAILURE") || responseStr.contains("EXAM_FOR_STUDENT_FAILURE")) {
        // Kiểm tra xem exam đã finished chưa và có submission không
        checkAndShowOldExamResult();
    }
}

void ExamTaking::loadQuestions()
{
    qDebug() << "=== loadQuestions ===";
    qDebug() << "examId:" << examId;
    qDebug() << "user_id:" << UserData::instance().getUserId();
    
    // Create new socket for this request
    if (tcpSocket->state() != QAbstractSocket::UnconnectedState) {
        tcpSocket->disconnect(this);
        tcpSocket->close();
    }
    
    tcpSocket->connectToHost(IPADDRESS, PORT);
    if (tcpSocket->waitForConnected(3000)) {
        QJsonObject json;
        json["exam_id"] = examId;
        json["user_id"] = UserData::instance().getUserId();
        
        QString jsonStr = QString(QJsonDocument(json).toJson(QJsonDocument::Compact));
        QString request = QString("CONTROL GET_EXAM_FOR_STUDENT\n%1").arg(jsonStr);
        
        qDebug() << "Request JSON:" << jsonStr;
        qDebug() << "Full request:" << request;
        
        tcpSocket->write(request.toUtf8());
        tcpSocket->flush();
        
        // Disconnect any existing connection first
        tcpSocket->disconnect(this);
        connect(tcpSocket, &QTcpSocket::readyRead, this, &ExamTaking::onReadyRead);
    } else {
        qDebug() << "ERROR: Cannot connect to server!";
        QMessageBox::critical(this, "Lỗi", "Không thể kết nối server để tải câu hỏi!");
        emit backToExamList();
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
    if (submissionId <= 0) return;
    
    QJsonObject q = questions[currentQuestionIndex].toObject();
    int questionId = q["id"].toInt();
    
    if (!userAnswers.contains(questionId)) return;
    
    // Use a separate socket for saving answers (fire and forget)
    QTcpSocket *answerSocket = new QTcpSocket(this);
    
    connect(answerSocket, &QTcpSocket::connected, [this, answerSocket, questionId]() {
        QJsonObject json;
        json["submission_id"] = submissionId;
        json["question_id"] = questionId;
        json["answer"] = userAnswers[questionId];
        
        QString request = QString("CONTROL SUBMIT_ANSWER\n%1").arg(QString(QJsonDocument(json).toJson(QJsonDocument::Compact)));
        answerSocket->write(request.toUtf8());
        answerSocket->flush();
    });
    
    connect(answerSocket, &QTcpSocket::readyRead, [answerSocket]() {
        QByteArray data = answerSocket->readAll();
        qDebug() << "Answer saved:" << data.left(100);
        answerSocket->close();
        answerSocket->deleteLater();
    });
    
    connect(answerSocket, &QTcpSocket::errorOccurred, [answerSocket](QAbstractSocket::SocketError) {
        qDebug() << "Answer save error:" << answerSocket->errorString();
        answerSocket->deleteLater();
    });
    
    answerSocket->connectToHost(IPADDRESS, PORT);
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

void ExamTaking::saveAllAnswers()
{
    // Count how many answers need to be saved
    int totalAnswers = 0;
    for (int i = 0; i < questions.size(); i++) {
        QJsonObject q = questions[i].toObject();
        int questionId = q["id"].toInt();
        if (userAnswers.contains(questionId)) {
            totalAnswers++;
        }
    }
    
    if (totalAnswers == 0) {
        qDebug() << "No answers to save";
        return;
    }
    
    // Use a counter to track completed saves
    QSharedPointer<int> completedCount(new int(0));
    
    // Save all answers
    for (int i = 0; i < questions.size(); i++) {
        QJsonObject q = questions[i].toObject();
        int questionId = q["id"].toInt();
        
        if (userAnswers.contains(questionId)) {
            // Use a separate socket for saving answers
            QTcpSocket *answerSocket = new QTcpSocket(this);
            
            connect(answerSocket, &QTcpSocket::connected, [this, answerSocket, questionId]() {
                QJsonObject json;
                json["submission_id"] = submissionId;
                json["question_id"] = questionId;
                json["answer"] = userAnswers[questionId];
                
                QString request = QString("CONTROL SUBMIT_ANSWER\n%1").arg(QString(QJsonDocument(json).toJson(QJsonDocument::Compact)));
                answerSocket->write(request.toUtf8());
                answerSocket->flush();
            });
            
            connect(answerSocket, &QTcpSocket::readyRead, [answerSocket, completedCount, totalAnswers]() {
                QByteArray data = answerSocket->readAll();
                qDebug() << "Answer saved before submit:" << data.left(100);
                answerSocket->close();
                (*completedCount)++;
                answerSocket->deleteLater();
            });
            
            connect(answerSocket, &QTcpSocket::errorOccurred, [answerSocket, completedCount, totalAnswers](QAbstractSocket::SocketError) {
                qDebug() << "Answer save error:" << answerSocket->errorString();
                (*completedCount)++;
                answerSocket->deleteLater();
            });
            
            answerSocket->connectToHost(IPADDRESS, PORT);
        }
    }
    
    // Wait for all sockets to complete (with timeout)
    QEventLoop loop;
    QTimer timeout;
    timeout.setSingleShot(true);
    timeout.setInterval(3000); // 3 second timeout
    
    connect(&timeout, &QTimer::timeout, &loop, &QEventLoop::quit);
    
    QTimer checkTimer;
    checkTimer.setInterval(50);
    connect(&checkTimer, &QTimer::timeout, [&loop, completedCount, totalAnswers]() {
        if (*completedCount >= totalAnswers) {
            loop.quit();
        }
    });
    
    timeout.start();
    checkTimer.start();
    loop.exec();
    checkTimer.stop();
    timeout.stop();
    
    qDebug() << "Saved" << *completedCount << "out of" << totalAnswers << "answers before submit";
}

void ExamTaking::submitExam()
{
    examTimer->stop();
    
    // Save all answers before submitting
    saveAllAnswers();
    
    // Reset socket state
    if (tcpSocket->state() != QAbstractSocket::UnconnectedState) {
        tcpSocket->disconnect(this);
        tcpSocket->close();
    }
    
    tcpSocket->connectToHost(IPADDRESS, PORT);
    if (tcpSocket->waitForConnected(3000)) {
        QJsonObject json;
        json["submission_id"] = submissionId;
        
        QString request = QString("CONTROL SUBMIT_EXAM\n%1").arg(QString(QJsonDocument(json).toJson(QJsonDocument::Compact)));
        qDebug() << "Submitting exam, submission_id:" << submissionId;
        tcpSocket->write(request.toUtf8());
        tcpSocket->flush();
        
        // Disconnect any existing connection first
        tcpSocket->disconnect(this);
        connect(tcpSocket, &QTcpSocket::readyRead, this, &ExamTaking::onReadyRead);
    } else {
        QMessageBox::critical(this, "Lỗi", "Không thể kết nối server để nộp bài!");
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
    
    // Build detailed result with question content
    QString details;
    details += "═══════════════════════════════════════\n";
    details += "              CHI TIẾT BÀI LÀM\n";
    details += "═══════════════════════════════════════\n\n";
    
    // Show detailed answers if available
    if (result.contains("answers")) {
        resultAnswers = result["answers"].toArray(); // Store for appeal dialog
        QJsonArray answers = resultAnswers;
        for (int i = 0; i < answers.size(); i++) {
            QJsonObject a = answers[i].toObject();
            bool isCorrect = a["is_correct"].toBool();
            QString status = isCorrect ? "✅ ĐÚNG" : "❌ SAI";
            QString userAns = a["user_answer"].toString().trimmed();
            QString correctAns = a["correct_option"].toString();
            QString content = a["content"].toString();
            
            // CRITICAL: Validate user_answer - must be empty or one of A, B, C, D
            if (!userAns.isEmpty() && userAns != "A" && userAns != "B" && userAns != "C" && userAns != "D") {
                qDebug() << "WARNING: Invalid user_answer detected:" << userAns << "- treating as empty";
                userAns = "";
                isCorrect = false; // Force incorrect if invalid answer
            }
            
            qDebug() << "Question" << (i+1) << "user_answer:" << userAns << "is_correct:" << isCorrect;
            
            details += QString("────────────────────────────────────────\n");
            details += QString("📌 CÂU %1: %2\n").arg(i + 1).arg(status);
            details += QString("────────────────────────────────────────\n");
            details += QString("Nội dung: %1\n\n").arg(content);
            details += QString("A. %1\n").arg(a["option_a"].toString());
            details += QString("B. %1\n").arg(a["option_b"].toString());
            details += QString("C. %1\n").arg(a["option_c"].toString());
            details += QString("D. %1\n\n").arg(a["option_d"].toString());
            
            // CRITICAL: Only show answer if it's valid (A, B, C, or D)
            if (userAns.isEmpty() || (userAns != "A" && userAns != "B" && userAns != "C" && userAns != "D")) {
                details += QString("👉 Bạn chưa trả lời\n");
            } else {
                details += QString("👉 Bạn chọn: %1\n").arg(userAns);
            }
            details += QString("✓ Đáp án đúng: %1\n\n").arg(correctAns);
        }
    } else {
        // Fallback: show basic info from questions array
        for (int i = 0; i < questions.size(); i++) {
            QJsonObject q = questions[i].toObject();
            int qId = q["id"].toInt();
            QString content = q["content"].toString();
            QString userAns = userAnswers.value(qId, "");
            
            details += QString("────────────────────────────────────────\n");
            details += QString("📌 CÂU %1:\n").arg(i + 1);
            details += QString("Nội dung: %1\n").arg(content);
            details += QString("A. %1\n").arg(q["option_a"].toString());
            details += QString("B. %1\n").arg(q["option_b"].toString());
            details += QString("C. %1\n").arg(q["option_c"].toString());
            details += QString("D. %1\n").arg(q["option_d"].toString());
            
            // CRITICAL: Only show answer if it's valid (A, B, C, or D)
            if (userAns.isEmpty() || (userAns != "A" && userAns != "B" && userAns != "C" && userAns != "D")) {
                details += QString("👉 Bạn chưa trả lời\n\n");
            } else {
                details += QString("👉 Bạn chọn: %1\n\n").arg(userAns);
            }
        }
    }
    
    details += "═══════════════════════════════════════\n";
    details += "Nếu bạn cho rằng có sai sót, hãy bấm\n";
    details += "'Khiếu nại kết quả' để gửi yêu cầu phúc tra.\n";
    details += "═══════════════════════════════════════\n";
    
    ui->textResultDetails->setText(details);
}

void ExamTaking::onBackToListClicked()
{
    emit backToExamList();
}

void ExamTaking::onAppealClicked()
{
    qDebug() << "=== onAppealClicked ===";
    qDebug() << "resultAnswers.size():" << resultAnswers.size();
    qDebug() << "submissionId:" << submissionId;
    qDebug() << "examId:" << examId;
    
    if (resultAnswers.isEmpty()) {
        QMessageBox::warning(this, "Lỗi", "Không có thông tin chi tiết bài làm để khiếu nại!");
        return;
    }
    
    qDebug() << "Creating AppealDialog...";
    AppealDialog dialog(submissionId, examId, resultAnswers, this);
    qDebug() << "Dialog created, calling exec()...";
    dialog.exec();
    qDebug() << "Dialog closed";
}

void ExamTaking::closeEvent(QCloseEvent *event)
{
    // Only warn if exam is in progress (not on result screen)
    if (ui->stackedWidget->currentIndex() == 0 && examTimer->isActive()) {
        QMessageBox::StandardButton reply = QMessageBox::warning(this,
            "Cảnh báo",
            "Bạn chưa hoàn thành bài thi!\n\n"
            "Các câu trả lời hiện tại đã được lưu tự động.\n"
            "Bạn có thể quay lại làm tiếp trong thời gian còn lại.\n\n"
            "Nếu hết thời gian mà chưa nộp bài, hệ thống sẽ tự động chấm điểm.\n\n"
            "Bạn có chắc muốn thoát?",
            QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::No) {
            event->ignore();
            return;
        }

        // Save all current answers before exiting
        saveAllAnswers();
    }

    event->accept();
}

void ExamTaking::checkAndShowOldExamResult()
{
    // Kiểm tra xem exam đã finished và có submission không
    QTcpSocket *checkSocket = new QTcpSocket(this);
    QSharedPointer<bool> connectionSuccess(new bool(false));
    
    connect(checkSocket, &QTcpSocket::connected, [this, checkSocket, connectionSuccess]() {
        *connectionSuccess = true;
        QJsonObject json;
        json["exam_id"] = examId;
        json["user_id"] = UserData::instance().getUserId();
        
        QString request = QString("CONTROL GET_SUBMISSION_STATUS\n%1")
            .arg(QString(QJsonDocument(json).toJson(QJsonDocument::Compact)));
        checkSocket->write(request.toUtf8());
        checkSocket->flush();
    });
    
    connect(checkSocket, &QTcpSocket::readyRead, [this, checkSocket]() {
        QByteArray response = checkSocket->readAll();
        QString responseStr(response);
        checkSocket->close();
        checkSocket->deleteLater();
        
        int jsonStart = responseStr.indexOf('{');
        if (jsonStart == -1) {
            QMessageBox::information(this, "Thông báo", "Bạn chưa tham gia bài thi này.");
            emit backToExamList();
            return;
        }
        
        QJsonDocument doc = QJsonDocument::fromJson(responseStr.mid(jsonStart).toUtf8());
        if (doc.isNull()) {
            QMessageBox::information(this, "Thông báo", "Bạn chưa tham gia bài thi này.");
            emit backToExamList();
            return;
        }
        
        QJsonObject obj = doc.object();
        int submissionId = obj["submission_id"].toInt();
        QString status = obj["status"].toString();
        
        if (submissionId > 0 && status == "submitted") {
            // Có submission đã nộp → hiển thị kết quả
            emit examFinished(submissionId);
        } else if (submissionId > 0 && status == "in_progress") {
            // Có submission nhưng chưa nộp → thông báo
            QMessageBox::information(this, "Thông báo", 
                "Bạn đã bắt đầu làm bài thi này nhưng chưa nộp bài.\n"
                "Vui lòng hoàn thành bài thi trong thời gian quy định.");
            emit backToExamList();
        } else {
            // Không có submission
            QMessageBox::information(this, "Thông báo", "Bạn chưa tham gia bài thi này.");
            emit backToExamList();
        }
    });
    
    connect(checkSocket, &QTcpSocket::errorOccurred, [this, checkSocket, connectionSuccess](QAbstractSocket::SocketError) {
        if (!(*connectionSuccess)) {
            // Chỉ hiển thị lỗi nếu chưa kết nối được
            QMessageBox::critical(this, "Lỗi", 
                QString("Không thể kết nối đến server!\n\n"
                        "Vui lòng kiểm tra:\n"
                        "1. Backend server đã được khởi động chưa?\n"
                        "2. IP và Port trong config.h có đúng không?\n"
                        "3. Firewall có chặn kết nối không?"));
        }
        checkSocket->deleteLater();
        emit backToExamList();
    });
    
    checkSocket->connectToHost(IPADDRESS, PORT);
    if (!checkSocket->waitForConnected(3000)) {
        if (!(*connectionSuccess)) {
            QMessageBox::critical(this, "Lỗi", 
                QString("Không thể kết nối đến server tại %1:%2!\n\n"
                        "Vui lòng kiểm tra:\n"
                        "1. Backend server đã được khởi động chưa?\n"
                        "2. IP và Port có đúng không?\n"
                        "3. Firewall có chặn kết nối không?")
                    .arg(IPADDRESS).arg(PORT));
        }
        checkSocket->deleteLater();
        emit backToExamList();
    }
}

void ExamTaking::loadExamResult(int submissionId)
{
    qDebug() << "=== loadExamResult ===";
    qDebug() << "submissionId:" << submissionId;
    
    // Reset state
    this->submissionId = submissionId;
    questions = QJsonArray();
    userAnswers.clear();
    currentQuestionIndex = 0;
    
    // Hiển thị loading state ngay - QUAN TRỌNG: Phải set result page trước
    ui->stackedWidget->setCurrentIndex(1); // Show result page (index 1)
    qDebug() << "Set result page, current index:" << ui->stackedWidget->currentIndex();
    
    // Set loading text
    ui->lblExamName->setText("Đang tải kết quả...");
    ui->lblResultScore->setText("...");
    ui->lblResultDetail->setText("Đang tải dữ liệu...");
    ui->textResultDetails->setText("Vui lòng đợi...");
    
    // Force update
    ui->stackedWidget->update();
    this->update();
    
    // Disconnect any existing connections
    if (tcpSocket->state() != QAbstractSocket::UnconnectedState) {
        tcpSocket->disconnect(this);
        tcpSocket->close();
    }
    
    // Connect to server to get exam result (async)
    tcpSocket->connectToHost(IPADDRESS, PORT);
    
    connect(tcpSocket, &QTcpSocket::connected, [this, submissionId]() {
        QJsonObject json;
        json["submission_id"] = submissionId;
        
        QString request = QString("CONTROL GET_EXAM_RESULT\n%1")
            .arg(QString(QJsonDocument(json).toJson(QJsonDocument::Compact)));
        tcpSocket->write(request.toUtf8());
        tcpSocket->flush();
    });
    
    connect(tcpSocket, &QTcpSocket::readyRead, [this]() {
        QByteArray response = tcpSocket->readAll();
        QString responseStr(response);
        tcpSocket->disconnect(this);
        tcpSocket->close();
        
        int jsonStart = responseStr.indexOf('{');
        if (jsonStart == -1) {
            QMessageBox::critical(this, "Lỗi", "Dữ liệu không hợp lệ!");
            emit backToExamList();
            return;
        }
        
        QJsonDocument doc = QJsonDocument::fromJson(responseStr.mid(jsonStart).toUtf8());
        if (doc.isNull()) {
            QMessageBox::critical(this, "Lỗi", "Không thể parse dữ liệu!");
            emit backToExamList();
            return;
        }
        
        QJsonObject resultObj = doc.object();
        
        // Extract exam info
        examName = resultObj["exam_name"].toString();
        ui->lblExamName->setText(examName);
        
        // Show result
        showResult(resultObj);
    });
    
    connect(tcpSocket, &QTcpSocket::errorOccurred, [this](QAbstractSocket::SocketError) {
        QMessageBox::critical(this, "Lỗi", "Không thể kết nối server!");
        tcpSocket->disconnect(this);
        emit backToExamList();
    });
    
    // Nếu không kết nối được trong 3 giây, báo lỗi
    if (!tcpSocket->waitForConnected(3000)) {
        QMessageBox::critical(this, "Lỗi", "Không thể kết nối server!");
        tcpSocket->disconnect(this);
        emit backToExamList();
    }
}

