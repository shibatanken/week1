#include "examroomdialog.h"
#include "ui_examroomdialog.h"
#include "createquestion.h"

#include <QDialog>
#include <QVBoxLayout>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include "config.h"

ExamRoomDialog::ExamRoomDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ExamRoomDialog),
    tcpSocket(new QTcpSocket(this))
{
    ui->setupUi(this);

    connect(ui->addQuestionButton, &QPushButton::clicked, this, &ExamRoomDialog::onAddQuestionClicked);
}

ExamRoomDialog::~ExamRoomDialog()
{
    delete ui;
}

void ExamRoomDialog::setRoomDetails(int roomId, const QString &roomName, const QString &description, const QString &timeLimit)
{
    examRoomId = roomId;
    ui->roomNameLabel->setText(roomName);
    ui->descriptionLabel->setText(description);
    ui->timeLimitLabel->setText("Thời gian: " + timeLimit);

    handleGetQuestionBank();
}

void ExamRoomDialog::onAddQuestionClicked()
{
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("Thêm câu hỏi");
    dialog->resize(700, 550);

    CreateQuestion *createQuestion = new CreateQuestion(dialog);
    createQuestion->setRoomId(examRoomId);

    QVBoxLayout *layout = new QVBoxLayout(dialog);
    layout->addWidget(createQuestion);
    dialog->setLayout(layout);

    connect(createQuestion, &CreateQuestion::questionCreated, [this]() {
        handleGetQuestionBank();
        emit roomUpdated();
    });

    dialog->exec();
}

void ExamRoomDialog::handleGetQuestionBank()
{
    QJsonObject json;
    json["room_id"] = examRoomId;

    QJsonDocument doc(json);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);

    QString dataString = QString("CONTROL GET_ROOM_QUESTION\n%1").arg(QString(jsonData));

    tcpSocket->connectToHost(IPADDRESS, 8081);
    if (tcpSocket->waitForConnected()) {
        tcpSocket->write(dataString.toUtf8());
        tcpSocket->flush();
    } else {
        qDebug() << "Failed to connect to server";
        return;
    }

    connect(tcpSocket, &QTcpSocket::readyRead, this, &ExamRoomDialog::handleGetQuestionBankResponse);
}

void ExamRoomDialog::handleGetQuestionBankResponse()
{
    QByteArray response = tcpSocket->readAll();
    QString responseString(response);

    qDebug() << "Get question bank response:" << responseString;

    if (responseString.startsWith("DATA JSON ROOM_QUESTION")) {
        int jsonStartIndex = responseString.indexOf('{');
        int jsonEndIndex = responseString.lastIndexOf('}');
        
        if (jsonStartIndex != -1 && jsonEndIndex != -1) {
            QString jsonString = responseString.mid(jsonStartIndex, jsonEndIndex - jsonStartIndex + 1);
            QJsonParseError parseError;
            QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonString.toUtf8(), &parseError);
            
            if (parseError.error == QJsonParseError::NoError) {
                QJsonObject jsonObj = jsonDoc.object();
                QJsonArray questionList = jsonObj["data"].toArray();

                ui->questionListWidget->clear();

                for (int i = 0; i < questionList.size(); ++i) {
                    QJsonObject question = questionList[i].toObject();
                    QString content = question["content"].toString();
                    int difficulty = question["difficulty"].toInt();
                    
                    QString diffText;
                    switch(difficulty) {
                        case 1: diffText = "[Dễ]"; break;
                        case 2: diffText = "[TB]"; break;
                        case 3: diffText = "[Khó]"; break;
                        default: diffText = "";
                    }

                    QListWidgetItem *item = new QListWidgetItem(diffText + " " + content);
                    ui->questionListWidget->addItem(item);
                }

                ui->questionCountLabel->setText("Số câu hỏi: " + QString::number(questionList.size()));
            }
        }
    }
}