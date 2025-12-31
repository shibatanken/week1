#include "examscores.h"
#include "ui_examscores.h"
#include "config.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>

ExamScores::ExamScores(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ExamScores),
    socket(new QTcpSocket(this)),
    currentExamId(0)
{
    ui->setupUi(this);

    // Setup table
    ui->scoresTable->setColumnCount(6);
    ui->scoresTable->setHorizontalHeaderLabels({"Name", "Email", "Score", "Correct", "Wrong", "Submitted At"});
    ui->scoresTable->horizontalHeader()->setStretchLastSection(true);
    ui->scoresTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->scoresTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(socket, &QTcpSocket::connected, this, &ExamScores::onSocketConnected);
    connect(socket, &QTcpSocket::readyRead, this, &ExamScores::onSocketReadyRead);
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onSocketError()));
}

ExamScores::~ExamScores()
{
    delete ui;
}

void ExamScores::loadScores(int examId)
{
    currentExamId = examId;
    ui->scoresTable->setRowCount(0);
    connectToServer();
}

void ExamScores::connectToServer()
{
    socket->connectToHost(SERVER_IP, SERVER_PORT);
}

void ExamScores::onSocketConnected()
{
    requestScores();
}

void ExamScores::requestScores()
{
    QString message = QString(
        "CONTROL GET_EXAM_SCORES\n"
        "{\"exam_id\": %1}"
    ).arg(currentExamId);

    socket->write(message.toUtf8());
    socket->flush();
}

void ExamScores::onSocketReadyRead()
{
    QString response = QString::fromUtf8(socket->readAll());

    // Parse response: "DATA JSON EXAM_SCORES\n{...}"
    QStringList parts = response.split("\n", QString::SkipEmptyParts);
    if (parts.size() >= 2) {
        QString jsonData = parts[1];
        displayScores(jsonData);
    }

    socket->disconnectFromHost();
}

void ExamScores::displayScores(const QString &jsonData)
{
    QJsonDocument doc = QJsonDocument::fromJson(jsonData.toUtf8());
    if (doc.isNull()) {
        QMessageBox::warning(this, "Error", "Failed to parse scores");
        return;
    }

    QJsonObject obj = doc.object();
    QJsonArray dataArray = obj["data"].toArray();

    ui->scoresTable->setRowCount(dataArray.size());
    ui->studentCountLabel->setText(QString("Total Students: %1").arg(dataArray.size()));

    for (int i = 0; i < dataArray.size(); ++i) {
        QJsonObject student = dataArray[i].toObject();

        QString name = student["name"].toString();
        QString email = student["email"].toString();
        double score = student["score"].toDouble();
        int correctCount = student["correct_count"].toInt();
        int wrongCount = student["wrong_count"].toInt();
        int totalQuestions = student["total_questions"].toInt();
        QString submittedAt = student["submitted_at"].toString();

        ui->scoresTable->setItem(i, 0, new QTableWidgetItem(name));
        ui->scoresTable->setItem(i, 1, new QTableWidgetItem(email));

        QTableWidgetItem *scoreItem = new QTableWidgetItem(QString("%1%").arg(score, 0, 'f', 2));
        if (score >= 80) {
            scoreItem->setForeground(Qt::darkGreen);
        } else if (score >= 50) {
            scoreItem->setForeground(QColor(255, 140, 0)); // Orange
        } else {
            scoreItem->setForeground(Qt::red);
        }
        ui->scoresTable->setItem(i, 2, scoreItem);

        ui->scoresTable->setItem(i, 3, new QTableWidgetItem(QString("%1/%2").arg(correctCount).arg(totalQuestions)));
        ui->scoresTable->setItem(i, 4, new QTableWidgetItem(QString("%1/%2").arg(wrongCount).arg(totalQuestions)));
        ui->scoresTable->setItem(i, 5, new QTableWidgetItem(submittedAt));
    }

    ui->scoresTable->resizeColumnsToContents();
}

void ExamScores::onSocketError()
{
    QMessageBox::critical(this, "Connection Error", socket->errorString());
}

void ExamScores::on_backButton_clicked()
{
    emit backToExamList();
}

void ExamScores::on_refreshButton_clicked()
{
    loadScores(currentExamId);
}
