#include "examresults.h"
#include "ui_examresults.h"
#include "config.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>

ExamResults::ExamResults(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ExamResults),
    socket(new QTcpSocket(this)),
    currentExamId(0),
    currentUserId(0)
{
    ui->setupUi(this);

    connect(socket, &QTcpSocket::connected, this, &ExamResults::onSocketConnected);
    connect(socket, &QTcpSocket::readyRead, this, &ExamResults::onSocketReadyRead);
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onSocketError()));
}

ExamResults::~ExamResults()
{
    delete ui;
}

void ExamResults::loadResults(int examId, int userId)
{
    currentExamId = examId;
    currentUserId = userId;
    connectToServer();
}

void ExamResults::connectToServer()
{
    socket->connectToHost(SERVER_IP, SERVER_PORT);
}

void ExamResults::onSocketConnected()
{
    requestResults();
}

void ExamResults::requestResults()
{
    QString message = QString(
        "CONTROL GET_EXAM_RESULTS\n"
        "{\"exam_id\": %1, \"user_id\": %2}"
    ).arg(currentExamId).arg(currentUserId);

    socket->write(message.toUtf8());
    socket->flush();
}

void ExamResults::onSocketReadyRead()
{
    QString response = QString::fromUtf8(socket->readAll());

    // Parse response: "DATA JSON EXAM_RESULTS\n{...}"
    QStringList parts = response.split("\n", QString::SkipEmptyParts);
    if (parts.size() >= 2) {
        QString jsonData = parts[1];
        displayResults(jsonData);
    }

    socket->disconnectFromHost();
}

void ExamResults::displayResults(const QString &jsonData)
{
    QJsonDocument doc = QJsonDocument::fromJson(jsonData.toUtf8());
    if (doc.isNull()) {
        QMessageBox::warning(this, "Error", "Failed to parse results");
        return;
    }

    QJsonObject obj = doc.object();

    // Extract data
    double score = obj["score"].toDouble();
    int correctCount = obj["correct_count"].toInt();
    int wrongCount = obj["wrong_count"].toInt();
    int totalQuestions = obj["total_questions"].toInt();
    QString examName = obj["exam_name"].toString();
    QString startTime = obj["start_time"].toString();
    QString endTime = obj["end_time"].toString();

    // Display in UI
    ui->examNameLabel->setText(examName);
    ui->scoreLabel->setText(QString("%1%").arg(score, 0, 'f', 2));
    ui->correctLabel->setText(QString("%1 / %2").arg(correctCount).arg(totalQuestions));
    ui->wrongLabel->setText(QString("%1 / %2").arg(wrongCount).arg(totalQuestions));
    ui->startTimeLabel->setText(startTime);
    ui->endTimeLabel->setText(endTime);

    // Set score color
    if (score >= 80) {
        ui->scoreLabel->setStyleSheet("color: green; font-size: 36px; font-weight: bold;");
    } else if (score >= 50) {
        ui->scoreLabel->setStyleSheet("color: orange; font-size: 36px; font-weight: bold;");
    } else {
        ui->scoreLabel->setStyleSheet("color: red; font-size: 36px; font-weight: bold;");
    }
}

void ExamResults::onSocketError()
{
    QMessageBox::critical(this, "Connection Error", socket->errorString());
}

void ExamResults::on_backButton_clicked()
{
    emit backToExamList();
}
