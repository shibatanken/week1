#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "userdata.h"
#include "config.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , tcpSocket(new QTcpSocket(this))
    , signupForm(new Signup(this))
    , signinForm(new Signin(this))
    , classListForm(new ClassList(this))
    , classDetailForm(new ClassDetail(this))
    , classMembersForm(new ClassMembers(this))
    , createExamForm(new CreateExam(this))
    , homeForm(new Home(this))
    , examTakingForm(new ExamTaking(this))
    , examListForm(new ExamList(this))
    , practiceModeForm(new PracticeMode(this))
    , appealManagerForm(new AppealManager(this))
    , adminDashboardForm(new AdminDashboard(this))
    , statisticsViewForm(new StatisticsView(this))
    , currentClassId(-1)
    , currentExamId(-1)
{
    ui->setupUi(this);

    ui->stackedWidget->addWidget(signupForm);
    ui->stackedWidget->addWidget(signinForm);
    ui->stackedWidget->addWidget(classListForm);
    ui->stackedWidget->addWidget(classDetailForm);
    ui->stackedWidget->addWidget(classMembersForm);
    ui->stackedWidget->addWidget(createExamForm);
    ui->stackedWidget->addWidget(homeForm);
    ui->stackedWidget->addWidget(examTakingForm);
    ui->stackedWidget->addWidget(examListForm);
    ui->stackedWidget->addWidget(practiceModeForm);
    ui->stackedWidget->addWidget(appealManagerForm);
    ui->stackedWidget->addWidget(adminDashboardForm);
    ui->stackedWidget->addWidget(statisticsViewForm);

    // Auth connections
    connect(signupForm, &Signup::showSignin, this, &MainWindow::showSignin);
    connect(signinForm, &Signin::showSignup, this, &MainWindow::showSignup);
    connect(signinForm, &Signin::showAppealManager, this, &MainWindow::showAppealManager);
    connect(signinForm, &Signin::loginSuccess, [this]() {
        QString role = UserData::instance().getRole();
        if (role == "admin") {
            showAdminDashboard();
        } else {
            showClassList();
        }
    });
    
    // Class connections
    connect(classListForm, &ClassList::showClassDetail, this, &MainWindow::showClassDetail);
    connect(classListForm, &ClassList::showAppealManager, this, &MainWindow::showAppealManager);
    connect(classListForm, &ClassList::showAdminDashboard, this, &MainWindow::showAdminDashboard);
    connect(classDetailForm, &ClassDetail::backToClassList, this, &MainWindow::showClassList);
    connect(classDetailForm, &ClassDetail::openClassMembers, this, &MainWindow::showClassMembers);
    connect(classMembersForm, &ClassMembers::backToClassDetail, [this]() {
        showClassDetail(currentClassId);
    });
    connect(classDetailForm, &ClassDetail::openCreateExam, this, &MainWindow::showCreateExam);
    connect(classDetailForm, &ClassDetail::openExamDetail, this, &MainWindow::showEditExam);
    connect(classDetailForm, &ClassDetail::startExamForStudent, this, &MainWindow::showExamTaking);
    connect(classDetailForm, &ClassDetail::openPracticeMode, this, &MainWindow::showPracticeMode);
    connect(classDetailForm, &ClassDetail::viewExamResult, this, &MainWindow::showExamResult);
    connect(createExamForm, &CreateExam::backToClassDetail, [this]() {
        showClassDetail(currentClassId);
    });
    
    // Exam connections
    connect(examTakingForm, &ExamTaking::backToExamList, [this]() {
        showClassDetail(currentClassId);
    });
    connect(examTakingForm, &ExamTaking::examFinished, this, &MainWindow::showExamResult);
    connect(examListForm, &ExamList::backToClassList, this, &MainWindow::showClassList);
    connect(examListForm, &ExamList::startExam, this, &MainWindow::showExamTaking);
    connect(examListForm, &ExamList::viewExamResult, this, &MainWindow::showExamResult);
    
    // Practice connections
    connect(practiceModeForm, &PracticeMode::backToClassDetail, [this]() {
        showClassDetail(currentClassId);
    });
    
    // Appeal connections
    connect(appealManagerForm, &AppealManager::backPressed, this, &MainWindow::showClassList);
    
    // Admin connections
    connect(adminDashboardForm, &AdminDashboard::logout, this, &MainWindow::showSignin);
    
    // Statistics connections
    connect(statisticsViewForm, &StatisticsView::backPressed, [this]() {
        showClassDetail(currentClassId);
    });
    
    // Logout connections
    connect(homeForm, &Home::logout, this, &MainWindow::showSignin);
    connect(classListForm, &ClassList::logout, this, &MainWindow::showSignin);

    ui->stackedWidget->setCurrentWidget(signinForm);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::showSignup() {
    ui->stackedWidget->setCurrentWidget(signupForm);
}

void MainWindow::showSignin() {
    UserData::instance().reset();
    ui->stackedWidget->setCurrentWidget(signinForm);
}

void MainWindow::showClassList() {
    classListForm->loadClassList();
    ui->stackedWidget->setCurrentWidget(classListForm);
}

void MainWindow::showClassDetail(int classId) {
    currentClassId = classId;
    classDetailForm->setClassId(classId);
    ui->stackedWidget->setCurrentWidget(classDetailForm);
}

void MainWindow::showClassMembers(int classId, QString className) {
    currentClassId = classId;
    classMembersForm->setClassInfo(classId, className);
    ui->stackedWidget->setCurrentWidget(classMembersForm);
}

void MainWindow::showCreateExam(int classId) {
    currentClassId = classId;
    createExamForm->setClassId(classId);
    ui->stackedWidget->setCurrentWidget(createExamForm);
}

void MainWindow::showEditExam(int examId) {
    currentExamId = examId;
    createExamForm->setExamId(examId);
    ui->stackedWidget->setCurrentWidget(createExamForm);
}

void MainWindow::showHome() {
    homeForm->updateUserInfo();
    ui->stackedWidget->setCurrentWidget(homeForm);
}

void MainWindow::showExamTaking(int examId, QString examName, int timeLimit) {
    currentExamId = examId;
    examTakingForm->setExamInfo(examId, examName, timeLimit);
    examTakingForm->startExam();
    ui->stackedWidget->setCurrentWidget(examTakingForm);
}

void MainWindow::showExamList() {
    examListForm->loadExams();
    ui->stackedWidget->setCurrentWidget(examListForm);
}

void MainWindow::showExamResult(int submissionId) {
    // Connect to server to get exam result
    tcpSocket->connectToHost(IPADDRESS, PORT);
    if (!tcpSocket->waitForConnected(3000)) {
        QMessageBox::critical(this, "Lỗi", "Không thể kết nối server!");
        return;
    }

    QJsonObject json;
    json["submission_id"] = submissionId;

    QString request = QString("CONTROL GET_EXAM_RESULT\n%1").arg(QString(QJsonDocument(json).toJson(QJsonDocument::Compact)));
    tcpSocket->write(request.toUtf8());
    tcpSocket->flush();

    if (!tcpSocket->waitForReadyRead(5000)) {
        QMessageBox::critical(this, "Lỗi", "Không nhận được phản hồi từ server!");
        tcpSocket->close();
        return;
    }

    QByteArray response = tcpSocket->readAll();
    QString responseStr(response);
    tcpSocket->close();

    // Parse response
    int jsonStart = responseStr.indexOf('{');
    if (jsonStart == -1) {
        QMessageBox::critical(this, "Lỗi", "Dữ liệu không hợp lệ!");
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(responseStr.mid(jsonStart).toUtf8());
    QJsonObject resultObj = doc.object();

    // Build result display
    QString examName = resultObj["exam_name"].toString();
    double score = resultObj["score"].toDouble();
    int correct = resultObj["correct_answers"].toInt();
    int total = resultObj["total_questions"].toInt();

    QString resultText = QString("<h2>%1</h2>").arg(examName);
    resultText += QString("<p><b>Điểm:</b> %1/%2</p>").arg(score, 0, 'f', 1).arg(total);
    resultText += QString("<p><b>Số câu đúng:</b> %1/%2</p><hr>").arg(correct).arg(total);

    QJsonArray answers = resultObj["answers"].toArray();
    for (int i = 0; i < answers.size(); i++) {
        QJsonObject answer = answers[i].toObject();
        QString content = answer["content"].toString();
        QString userAnswer = answer["user_answer"].toString();
        QString correctOption = answer["correct_option"].toString();
        bool isCorrect = answer["is_correct"].toBool();

        QString optionA = answer["option_a"].toString();
        QString optionB = answer["option_b"].toString();
        QString optionC = answer["option_c"].toString();
        QString optionD = answer["option_d"].toString();

        resultText += QString("<p><b>Câu %1:</b> %2</p>").arg(i + 1).arg(content);
        resultText += QString("<p>A. %1<br>B. %2<br>C. %3<br>D. %4</p>").arg(optionA).arg(optionB).arg(optionC).arg(optionD);
        resultText += QString("<p><b>Đáp án của bạn:</b> %1 %2</p>")
                          .arg(userAnswer.isEmpty() ? "(Không trả lời)" : userAnswer)
                          .arg(isCorrect ? "✓" : "✗");
        resultText += QString("<p><b>Đáp án đúng:</b> %1</p><hr>").arg(correctOption);
    }

    // Show in a message box with scrollable text
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Kết quả bài thi");
    msgBox.setTextFormat(Qt::RichText);
    msgBox.setText(resultText);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setStyleSheet("QLabel{min-width: 600px; min-height: 400px;}");
    msgBox.exec();
}


void MainWindow::showPracticeMode(int classId, QString className) {
    currentClassId = classId;
    practiceModeForm->setClassInfo(classId, className);
    ui->stackedWidget->setCurrentWidget(practiceModeForm);
}

void MainWindow::showAppealManager() {
    bool isTeacher = UserData::instance().isTeacher();
    appealManagerForm->setMode(isTeacher);
    appealManagerForm->loadAppeals();
    ui->stackedWidget->setCurrentWidget(appealManagerForm);
}

void MainWindow::showAdminDashboard() {
    adminDashboardForm->loadData();
    ui->stackedWidget->setCurrentWidget(adminDashboardForm);
}

void MainWindow::showStatistics(int examId) {
    currentExamId = examId;
    statisticsViewForm->loadExamStatistics(examId);
    ui->stackedWidget->setCurrentWidget(statisticsViewForm);
}

void MainWindow::showClassStatistics(int classId) {
    currentClassId = classId;
    statisticsViewForm->loadClassStatistics(classId);
    ui->stackedWidget->setCurrentWidget(statisticsViewForm);
}
